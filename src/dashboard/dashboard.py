from flask import *
from config import *
#import cv2
import time
import redis
import io
import base64
#import picamera
app = Flask(__name__)
app.config.from_object('config.DevConfig')
@app.route('/', methods=['GET'])

def index():
    return render_template('index.html')

@app.route('/stream', methods=['GET'])
def stream():
    def image_gen():
        red = redis.StrictRedis(host='localhost', port=6379, db=0)

        frame =red.get('img')
        while True:
            yield(b'--frame\r\n'
                  b'Content-Type: image/jpeg\r\n\r\n' + base64.b64decode(frame) + b'\r\n')
        
    return Response(image_gen(), mimetype='multipart/x-mixed-replace; boundary=frame')

@app.route('/api', methods=['GET', 'POST'])
def api_base():
    if not request.is_xhr:
        abort(404)
    else:
        return jsonify(ack='ok')

@app.route('/api/image', methods=['GET', 'POST'])
def api_image():
    if not request.is_xhr and not request.is_json:
        abort(404)
    if request.method == 'GET':
        try:
            red = redis.StrictRedis(host='localhost', port=6379, db=0)
            img_data_b64 = red.get('img')
            return jsonify(ack='ok', img=img_data_b64)
        except KeyError as e:
            return jsonify(ack='bad', img='')
    if request.method == 'POST': # Image update
        try:
            in_json = request.get_json(force=True)
            img_data_b64 = in_json['img']
            red = redis.StrictRedis(host='localhost', port=6379, db=0)
            red.set('img', img_data_b64)
            return jsonify(ack='ok')
        except KeyError as e:
            return jsonify(ack='bad', msg='Empty image field')

@app.route('/api/diagnostics', methods=['GET', 'POST'])
def api_pidiag(): # Raspberry Pi Diagnostics
    if not request.is_xhr and not request.is_json:
        abort(404)
    if request.method == 'GET':
        try:
            r = redis.StrictRedis(host='localhost', port=6379, db=0)
            cpu_percent = r.get('pi_cpu_percent')
            total_memory = r.get('pi_total_memory')
            used_memory = str(int(total_memory) - int(r.get('pi_available_memory')))
            percent_memory_used = r.get('pi_percent_memory_used')
            response = { 'ack' : 'ok', 'pi' : { 'cpu_percent' : cpu_percent, 'used_memory' : used_memory, 'total_memory' : total_memory, 'percent_memory_used' : percent_memory_used } }
            print 'Success'
            return jsonify(response)
        except Exception as e:
            print '[-] ERROR: ' + e.message
    if request.method == 'POST':
        try:
            r = redis.StrictRedis(host='localhost', port=6379, db=0)
            in_json = request.get_json(force=True)
            r.set('pi_cpu_percent', in_json['cpu_percent'])
            r.set('pi_total_memory' , in_json['memory']['total'])
            r.set('pi_available_memory', in_json['memory']['available'])
            r.set('pi_percent_memory_used', in_json['memory']['percent_used'])
            return jsonify(ack='ok')
        except KeyError as e:
            return jsonify(ack='bad', msg='A chosen key does not exist!')

if __name__=='__main__':
    app.run(host=app.config['HOST'], port=app.config['PORT'])
