import RPi.GPIO as GPIO
import tkinter as tk
from tkinter import ttk

# === GPIO Setup ===
GPIO.setmode(GPIO.BCM)
LED_PINS = [17, 27, 22]  # BCM pin numbers

# Initialize pins as output
for led in LED_PINS:
    GPIO.setup(led, GPIO.OUT)

# Create PWM objects
pwm_objects = [GPIO.PWM(led, 1000) for led in LED_PINS]  # 1kHz frequency
for pwm in pwm_objects:
    pwm.start(0)  # start with LEDs off (0% duty cycle)


# === Functions ===
def adjust_brightness(index, duty):
    """Adjust LED brightness based on slider value"""
    pwm_objects[index].ChangeDutyCycle(float(duty))


def shutdown():
    """Clean exit on close"""
    for pwm in pwm_objects:
        pwm.stop()
    GPIO.cleanup()
    root.destroy()


# === GUI Setup ===
root = tk.Tk()
root.title("LED Brightness Control")
root.geometry("400x300")
root.configure(bg="#2b2b40")

header = tk.Label(root, text="LED Brightness Controller",
                  font=("Arial", 16, "bold"), bg="#2b2b40", fg="white")
header.pack(pady=12)

led_labels = ["Red LED", "Green LED", "Blue LED"]

for idx, label in enumerate(led_labels):
    tk.Label(root, text=label, bg="#2b2b40", fg="white").pack()
    slider = ttk.Scale(root, from_=0, to=100, orient="horizontal",
                       command=lambda val, i=idx: adjust_brightness(i, val))
    slider.pack(pady=6)

# Exit button
exit_button = tk.Button(root, text="Exit", command=shutdown,
                        bg="darkred", fg="white", font=("Arial", 12, "bold"))
exit_button.pack(pady=12)

root.protocol("WM_DELETE_WINDOW", shutdown)
root.mainloop()
