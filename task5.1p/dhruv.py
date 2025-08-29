import RPi.GPIO as GPIO
import tkinter as tk

GPIO.setmode(GPIO.BCM)
led_pins = {"Red": 3, "Green": 5, "White": 22}
for pin in led_pins.values():
    GPIO.setup(pin, GPIO.OUT)
    GPIO.output(pin, GPIO.LOW)

def turn_on_led(color):
    for led_color, pin in led_pins.items():
        GPIO.output(pin, GPIO.HIGH if led_color == color else GPIO.LOW)

def exit_app():
    GPIO.cleanup()
    root.destroy()

root = tk.Tk()
root.title("Raspberry Pi LED Control")
