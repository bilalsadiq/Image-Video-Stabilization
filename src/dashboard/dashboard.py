from flask import *
from config import *
import cv2
import time
import io
import picamera
app = Flask(__name__)
app.config.from_object('config.DevConfig')

@app.route('/', methods=['GET'])

def index():
    return render_template('index.html')

@app.route('/stream', methods=['GET'])
def stream():
    def image_gen():
        stream = io.BytesIO()
        cam = picamera.PiCamera()
        time.sleep(5)
        while True:
            cam.capture(stream, 'jpeg')
            yield(b'--frame\r\n'
                  b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')
        
    return Response(image_gen(), mimetype='multipart/x-mixed-replace; boundary=frame')
if __name__=='__main__':
    app.run(host=app.config['HOST'], port=app.config['PORT'])


