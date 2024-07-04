#![no_std]
#![no_main]

use arduino_hal::prelude::*;
use panic_halt as _;

#[arduino_hal::entry]
fn main() -> ! {
    let dp = arduino_hal::Peripherals::take().unwrap();
    let pins = arduino_hal::pins!(dp);

    let mut serial = arduino_hal::default_serial!(dp, pins, 115200);

    let mut delay = arduino_hal::Delay::new();
    let mut led = pins.d13.into_output();
    let mut recv = b'5';
    loop {
        serial.write_byte(recv);
        delay.delay_ms(1000u16);
        recv = serial.read_byte();
        led.set_high();
        delay.delay_ms(500u16);
        led.set_low();
    }
}