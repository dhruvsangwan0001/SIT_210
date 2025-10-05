import speech_recognition as sr      
import RPi.GPIO as GPIO             
import time                          
import tkinter as tk                 
import threading                     

LED_PIN = 14  

# Set GPIO mode and configure LED pin as output
GPIO.setmode(GPIO.BCM)
GPIO.setup(LED_PIN, GPIO.OUT)
r = sr.Recognizer()  # Create a speech recognizer object

# GUI Setup
root = tk.Tk()                         # Initialize Tkinter window
root.title("Voice Controlled LED")     # Window title
root.geometry("300x200")               # Window size
root.configure(bg="#2c3e50")           # Dark background color

# Heading label
title_label = tk.Label(
    root,
    text="Voice Controlled LED",
    font=("Arial", 16, "bold"),
    fg="white",
    bg="#2c3e50"
)
title_label.pack(pady=10)

# LED Status label
status_label = tk.Label(
    root,
    text="LED Status: OFF",
    font=("Arial", 14),
    fg="#f1c40f",
    bg="#2c3e50"
)
status_label.pack(pady=20)

# Function to update LED status label
def update_status(state):
    status_label.config(text=f"LED Status: {state}")

# Exit Function
def exit_app():
    """Cleans up GPIO and closes the app window."""
    GPIO.cleanup()  # Reset GPIO pins
    root.destroy()  # Close GUI window

# Exit button in GUI
exit_button = tk.Button(
    root,
    text="Exit",
    font=("Arial", 12, "bold"),
    bg="#e74c3c",
    fg="white",
    width=10,
    command=exit_app
)
exit_button.pack(pady=20)

# Voice Command Functions
def listen_for_command():
    """
    Listens through the microphone and converts speech to text.
    Returns the recognized command or None if not understood.
    """
    with sr.Microphone(device_index=1) as source:  # Use the microphone as input
        print("🎙️ Say 'on' or 'off'...")
        audio = r.listen(source)  # Capture audio input

        try:
            command = r.recognize_google(audio).lower()
            print(f"You said: {command}")
            return command
        except sr.UnknownValueError:
            print(" Could not understand audio")
            return None
        except sr.RequestError as e:
            print(f" API error: {e}")
            return None

def control_led(command):
    """
    Controls the LED based on the spoken command.
    Accepts 'on' or 'off' as valid commands.
    """
    if "on" in command:
        GPIO.output(LED_PIN, GPIO.HIGH)  # Turn LED ON
        update_status("ON")
        print(" LED turned ON")
    elif "off" in command:
        GPIO.output(LED_PIN, GPIO.LOW)   # Turn LED OFF
        update_status("OFF")
        print(" LED turned OFF")
    else:
        print(" Command not recognized as 'on' or 'off'")

def voice_loop():
    """
    Continuously listens for voice commands and controls the LED.
    Runs in a background thread so GUI remains responsive.
    """
    while True:
        cmd = listen_for_command()  # Get spoken command
        if cmd:
            control_led(cmd)        # Execute command
        time.sleep(1)               # Short delay to avoid overlap


# Run Voice Thread
# Start the voice recognition loop in a background thread
threading.Thread(target=voice_loop, daemon=True).start()
# Run the GUI Main Loop
try:
    root.mainloop() 
except KeyboardInterrupt:
    print("Exiting...")
    GPIO.cleanup()
