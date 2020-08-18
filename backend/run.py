from gevent.pywsgi import WSGIServer
from index import app
from gevent import monkey
monkey.patch_all()

http_server = WSGIServer(('0.0.0.0', 5050), app)
print('listen at port 5050')
http_server.serve_forever()
