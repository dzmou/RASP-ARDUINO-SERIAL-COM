"""
app.py — Flask entry point
"""

from flask import Flask
from flask_cors import CORS
from config import Config
from serial_handler import SerialHandler
from csv_logger import CsvLogger
from routes import register_routes

# ── App factory ───────────────────────────────────────────────
def create_app():
    app = Flask(__name__)
    app.config.from_object(Config)
    CORS(app, origins=Config.CORS_ORIGINS)

    # ── Shared services ──
    serial = SerialHandler(Config)
    csv    = CsvLogger(Config)

    # Wire CSV logging to incoming stream data
    serial.on_data(csv.log)

    # Start serial background thread
    serial.start()

    # Attach to app context
    app.serial = serial
    app.csv    = csv

    # Register blueprints
    register_routes(app)

    return app


if __name__ == "__main__":
    app = create_app()
    app.run(host=Config.HOST, port=Config.PORT, debug=Config.DEBUG)
