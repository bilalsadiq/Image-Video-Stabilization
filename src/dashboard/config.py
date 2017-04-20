class BaseConfig(object):
    PORT = 80
    HOST = '0.0.0.0'

class DevConfig(BaseConfig):
    DEBUG = True
    PORT = 8080
    ABS_PATH_TO_VID='/home/admin/projects/flask/dashboard/static/Shaky_Footage_stabilized.mp4'
    
