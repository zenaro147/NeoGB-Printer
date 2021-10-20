////minimal RGB LED support by Raphaël BOICHOT
void RGB_led_ON(int COLOR)
{
#ifdef COMMON_ANODE
      digitalWrite(COLOR, LOW);
#endif
#ifdef COMMON_CATHODE
      digitalWrite(COLOR, HIGH);
#endif
}

void RGB_led_OFF(int COLOR)
{
#ifdef COMMON_ANODE
      digitalWrite(COLOR, HIGH);
#endif
#ifdef COMMON_CATHODE
      digitalWrite(COLOR, LOW);
#endif
}

void RGB_blink(int COLOR, int number, int delay_on, int delay_off)
{
    for (int j = 1; j <= number; j++) {
#ifdef COMMON_ANODE
      digitalWrite(COLOR, LOW);
#endif
#ifdef COMMON_CATHODE
      digitalWrite(COLOR, HIGH);
#endif
      delay(delay_on);
#ifdef COMMON_ANODE
      digitalWrite(COLOR, HIGH);
#endif
#ifdef COMMON_CATHODE
      digitalWrite(COLOR, LOW);
#endif
      delay(delay_off);
    }
}

void RGB_blink_white(int number, int delay_on, int delay_off)
{
    for (int j = 1; j <= number; j++) {
#ifdef COMMON_ANODE
      digitalWrite(LED_STATUS_RED, LOW);
      digitalWrite(LED_STATUS_GREEN, LOW);
      digitalWrite(LED_STATUS_BLUE, LOW);
#endif
#ifdef COMMON_CATHODE
      digitalWrite(LED_STATUS_RED, HIGH);
      digitalWrite(LED_STATUS_GREEN, HIGH);
      digitalWrite(LED_STATUS_BLUE, HIGH);
#endif
      delay(delay_on);
#ifdef COMMON_ANODE
      digitalWrite(LED_STATUS_RED, HIGH);
      digitalWrite(LED_STATUS_GREEN, HIGH);
      digitalWrite(LED_STATUS_BLUE, HIGH);
#endif
#ifdef COMMON_CATHODE
      digitalWrite(LED_STATUS_RED, LOW);
      digitalWrite(LED_STATUS_GREEN, LOW);
      digitalWrite(LED_STATUS_BLUE, LOW);
#endif
      delay(delay_off);
    }
}

void RGB_blink_magenta(int number, int delay_on, int delay_off)
{
    for (int j = 1; j <= number; j++) {
#ifdef COMMON_ANODE
      digitalWrite(LED_STATUS_RED, LOW);
      digitalWrite(LED_STATUS_BLUE, LOW);
#endif
#ifdef COMMON_CATHODE
      digitalWrite(LED_STATUS_RED, HIGH);
      digitalWrite(LED_STATUS_BLUE, HIGH);
#endif
      delay(delay_on);
#ifdef COMMON_ANODE
      digitalWrite(LED_STATUS_RED, HIGH);
        digitalWrite(LED_STATUS_BLUE, HIGH);
#endif
#ifdef COMMON_CATHODE
      digitalWrite(LED_STATUS_RED, LOW);
      digitalWrite(LED_STATUS_BLUE, LOW);
#endif
      delay(delay_off);
    }
}

void RGB_init()
{
#ifdef COMMON_ANODE
      digitalWrite(LED_STATUS_RED, HIGH);
      digitalWrite(LED_STATUS_GREEN, HIGH);
      digitalWrite(LED_STATUS_BLUE, HIGH);
#endif
#ifdef COMMON_CATHODE
      digitalWrite(LED_STATUS_RED, LOW);
      digitalWrite(LED_STATUS_GREEN, LOW);
      digitalWrite(LED_STATUS_BLUE, LOW);
#endif
}
