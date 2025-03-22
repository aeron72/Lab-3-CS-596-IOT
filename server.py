from flask import Flask, request, render_template_string
import threading
import time
import datetime

app = Flask(__name__)

# Store latest sensor data (initialize to 0 instead of "N/A")
sensor_data = {"temperature": "0.0", "humidity": "0.0"}

# HTML template with real-time printing effect
html_template = """
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 Sensor Data</title>
    <style>
        body { font-family: Arial, sans-serif; text-align: center; margin-top: 50px; background-color: #f4f4f4; }
        h1 { color: #007BFF; }
        p { font-size: 20px; font-family: monospace; }
        .log { text-align: left; margin: auto; width: 60%; background: #fff; padding: 10px; border-radius: 5px; box-shadow: 0px 0px 10px rgba(0, 0, 0, 0.1); }
    </style>
    <script>
        function refreshData() {
            fetch(window.location.href)
                .then(response => response.text())
                .then(html => document.body.innerHTML = html);
        }
        setInterval(refreshData, 2000); // Refresh every 2 seconds
    </script>
</head>
<body>
    <h1>ESP32 Sensor Data</h1>
    <div class="log">
        <p><strong>Live Data:</strong></p>
        {% for log in logs %}
            <p>{{ log }}</p>
        {% endfor %}
    </div>
</body>
</html>
"""

# Store last 20 logs to print on the webpage (like serial monitor output)
log_buffer = []

@app.route("/", methods=["GET"])
def display_data():
    return render_template_string(html_template, logs=log_buffer)

@app.route("/update", methods=["GET"])
def receive_data():
    temp = request.args.get("temp")
    humidity = request.args.get("humidity")

    if temp is None or humidity is None:
        return "Error: Missing temperature or humidity values", 400

    # Update sensor data
    sensor_data["temperature"] = temp
    sensor_data["humidity"] = humidity

    # Format timestamp for logs
    timestamp = datetime.datetime.now().strftime("%d/%b/%Y %H:%M:%S")

    # Log data in structured format (like serial monitor)
    log_entry = f'[{timestamp}] Temperature: {temp}°C, Humidity: {humidity}%'
    
    # Store up to 20 log entries (acts like a rolling buffer)
    if len(log_buffer) >= 20:
        log_buffer.pop(0)
    log_buffer.append(log_entry)

    print(log_entry)  # Print in EC2 terminal

    return f"Data received: Temperature={temp}°C, Humidity={humidity}%", 200

# Background thread to simulate continuous serial monitor-like printing
def print_sensor_data():
    while True:
        timestamp = datetime.datetime.now().strftime("%d/%b/%Y %H:%M:%S")
        log_entry = f'[{timestamp}] Temperature: {sensor_data["temperature"]}°C, Humidity: {sensor_data["humidity"]}%'
        
        if len(log_buffer) >= 20:
            log_buffer.pop(0)
        log_buffer.append(log_entry)
        
        print(log_entry)  # Print in EC2 terminal every 5 sec
        
        time.sleep(5)

# Start the background thread
threading.Thread(target=print_sensor_data, daemon=True).start()

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000, debug=True)
