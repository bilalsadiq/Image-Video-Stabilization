from flask import *
from config import *
app = Flask(__name__)
app.config.from_object('config.DevConfig')

@app.route('/', methods=['GET'])

def index():
    return render_template('index.html')

if __name__=='__main__':
    app.run(host=app.config['HOST'], port=app.config['PORT'])

