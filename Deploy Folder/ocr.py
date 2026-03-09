import easyocr
import sys
from http.server import HTTPServer, BaseHTTPRequestHandler
from urllib.parse import urlparse, parse_qs

reader = easyocr.Reader(['en'], gpu=False)
print("OCR server ready", flush=True)

class Handler(BaseHTTPRequestHandler):
    def do_GET(self):
        params = parse_qs(urlparse(self.path).query)
        path = params.get('path', [''])[0]
        results = reader.readtext(path, detail=0, allowlist='0123456789')
        text = ''.join(results)
        self.send_response(200)
        self.end_headers()
        self.wfile.write(text.encode())

    def log_message(self, format, *args):
        pass  # suppress request logs

HTTPServer(('localhost', 9119), Handler).serve_forever()