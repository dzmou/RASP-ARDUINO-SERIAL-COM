import serial
import serial.tools.list_ports
import threading
import time

class SerialBackend:
    """Handles serial communication logic independently of the GUI."""
    def __init__(self, callback):
        self.ser = None
        self.callback = callback
        self.running = False
        self.thread = None
        self.port = None
        self.baud = 9600

    def list_ports(self):
        """Returns a list of available serial port names."""
        return [p.device for p in serial.tools.list_ports.comports()]

    def connect(self, port, baud=9600):
        """Attempts to open a serial connection and starts the read thread."""
        self.disconnect()
        try:
            self.ser = serial.Serial(port, baud, timeout=0.1)
            self.port = port
            self.baud = baud
            self.running = True
            self.thread = threading.Thread(target=self._listen, daemon=True)
            self.thread.start()
            return True, "Connected"
        except Exception as e:
            return False, str(e)

    def disconnect(self):
        """Stops the read thread and closes the serial port."""
        self.running = False
        if self.ser and self.ser.is_open:
            try:
                self.ser.close()
            except:
                pass
        if self.thread and self.thread.is_alive():
            # Listen loop usually ends quickly due to 'running' flag
            self.thread.join(timeout=0.2)
        self.ser = None

    def write(self, msg):
        """Sends a message to the serial port with a newline."""
        return self.write_raw(msg + '\n')

    def write_raw(self, msg):
        """Sends a message to the serial port without extra characters (e.g. no newline)."""
        if self.ser and self.ser.is_open:
            try:
                self.ser.write(msg.encode('utf-8'))
                return True
            except:
                return False
        return False

    def write_nb_1(self):
        """Sends a 1 number to the serial port without extra characters (e.g. no newline)."""
        if self.ser and self.ser.is_open:
            try:
                self.ser.write(1) # send 1 number without newline
                return True
            except:
                return False
        return False

    def _listen(self):
        """Background thread loop for reading serial data."""
        while self.running:
            if self.ser and self.ser.is_open:
                try:
                    if self.ser.in_waiting > 0:
                        line = self.ser.readline().decode('utf-8', errors='replace').rstrip()
                        if line:
                            self.callback(f"\t: {line}")
                except Exception as e:
                    self.callback(f"SYSTEM ERROR: {e}")
                    self.running = False
                    break
            time.sleep(0.01)

    def wakeup_datahog(self):
        WAKEUP_CHAR = b'1'  # Any numeric key (0-9) works
        ESC_CHAR = b'\x1b'  # ESC character to return to sleep/log mode

        if self.ser and self.ser.is_open:
            try:
                self._update_display("Initiating wake-up sequence (duration: 12 seconds)...")
            
                start_time = time.time()
                # Send numeric key repeatedly for slightly over the 10s wake-up period
                while time.time() - start_time < 12:
                    self.ser.write(WAKEUP_CHAR)
                    time.sleep(1)  # Interval between attempts
                    
                    # Check if device responded with the Main Menu
                    if self.ser.in_waiting > 0:
                        response = self.ser.read(self.ser.in_waiting).decode('ascii', errors='ignore')
                        if "Main Menu" in response or response.strip():
                            self._update_display("Device Awakened: Main Menu accessed.")
                            return True
                
                self._update_display("Wake-up attempt complete.")
            except Exception as e:
                self._update_display(f"Connection Error: {e}")
        return False

    def sleep_datahog(self):
        # Sending ESC returns the device to sleep/log mode
        ESC_CHAR = b'\x1b'  # ESC character to return to sleep/log mode
        if self.ser and self.ser.is_open:
            try:
                self.ser.write(ESC_CHAR)
                self._update_display("ESC sent: Device returning to sleep/logging mode.")
                return True
            except Exception as e:
                self._update_display(f"Error sending sleep command: {e}")
        return False
