
from flask import Flask, render_template_string
import folium
import paho.mqtt.client as mqtt
import json

# Initialize Flask app
app = Flask(__name__)
# Global variables to store car data
car_lat = 0.0
car_lng = 0.0
distance_cm = None
proximity_alert = False

def on_message(client, userdata, msg):
    global car_lat, car_lng, distance_cm, proximity_alert

    try:
        topic = msg.topic
        payload = msg.payload.decode("utf-8")
        data = json.loads(payload)

        if topic == "car/001/gps":
            if data.get("lat") is not None and data.get("lng") is not None:
                car_lat = data["lat"]
                car_lng = data["lng"]

        elif topic == "car/001/proximity":
            distance_cm = data.get("distance_cm")
            proximity_alert = data.get("alert") == "object_too_close"

        print("[MQTT] Received from {topic}: {data}")

    except Exception as e:
        print("[ERROR] Failed to process MQTT message: {e}")

# MQTT Client Setup

client = mqtt.Client()
client.on_message = on_message

# Connect to the local MQTT broker (Raspberry Pi)
client.connect("localhost", 1883, 60)

# Subscribe to relevant topics
client.subscribe("car/001/gps")
client.subscribe("car/001/proximity")

# Start MQTT loop in background
client.loop_start()

# HTML Template

HTML_TEMPLATE = """
<!DOCTYPE html>
<html lang="en">
<head>
    <meta http-equiv="refresh" content="3">
    <meta charset="UTF-8">
    <title>Smart Car Tracker</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f5f5f5;
            margin: 20px;
        }
        h2 {
            color: #333;
        }
        p {
            font-size: 16px;
            color: #444;
        }
        .alert {
            color: red;
            font-weight: bold;
        }
        .map {
            margin-top: 20px;
        }
    </style>
</head>
<body>
    <h2>🚗 Smart Car Location & Proximity Monitor</h2>
    <p>
        <strong>Latitude:</strong> {{ lat }} |
        <strong>Longitude:</strong> {{ lng }} |
        <strong>Distance:</strong> {{ dist }} cm |
        <strong>Status:</strong> 
        {% if prox %}
            <span class="alert">⚠️ Object Too Close!</span>
        {% else %}
            Safe ✅
        {% endif %}
    </p>
    <div class="map">{{ map|safe }}</div>
</body>
</html>
"""


# Flask Route
@app.route("/")
def index():
    """Render live car tracking data on a map."""
    global car_lat, car_lng, distance_cm, proximity_alert

    # Create a folium map centered on the latest coordinates
    folium_map = folium.Map(location=[car_lat, car_lng], zoom_start=15)
    folium.Marker(
        [car_lat, car_lng],
        tooltip="Car Location",
        icon=folium.Icon(color="blue", icon="car", prefix="fa"),
    ).add_to(folium_map)

    # Render the map and inject data into HTML template
    return render_template_string(
        HTML_TEMPLATE,
        lat=car_lat,
        lng=car_lng,
        dist=distance_cm or "N/A",
        prox=proximity_alert,
        map=folium_map._repr_html_(),
    )

# Main Entry Point

if __name__ == "__main__":
    print("[INFO] Starting Smart Car Tracker Dashboard..........")
    print("[INFO] Flask running on http://localhost:5000")
    app.run(host="0.0.0.0", port=5000, debug=True)
