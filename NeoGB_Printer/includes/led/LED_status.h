////minimal RGB LED support by Raphaël BOICHOT
void LED_led_ON(int COLOR1){
  #ifdef COMMON_ANODE
    digitalWrite(COLOR1, LOW);
  #endif
  #if defined(LED_STATUS_PIN) || defined(COMMON_CATHODE)
    digitalWrite(COLOR1, HIGH);
  #endif
}
void LED_led_ON(int COLOR1, int COLOR2){
  #ifdef COMMON_ANODE
    digitalWrite(COLOR1, LOW);
    digitalWrite(COLOR2, LOW);
  #endif
  #ifdef COMMON_CATHODE
    digitalWrite(COLOR1, HIGH);
    digitalWrite(COLOR2, HIGH);
  #endif
}
void LED_led_ON(int COLOR1, int COLOR2, int COLOR3){
  #ifdef COMMON_ANODE
    digitalWrite(COLOR1, LOW);
    digitalWrite(COLOR2, LOW);
    digitalWrite(COLOR3, LOW);
  #endif
  #ifdef COMMON_CATHODE
    digitalWrite(COLOR1, HIGH);
    digitalWrite(COLOR2, HIGH);
    digitalWrite(COLOR3, HIGH);
  #endif
}

void LED_led_OFF(int COLOR1)
{
  #ifdef COMMON_ANODE
    digitalWrite(COLOR1, HIGH);
  #endif
  #if defined(LED_STATUS_PIN) || defined(COMMON_CATHODE)
    digitalWrite(COLOR1, LOW);
  #endif
}
void LED_led_OFF(int COLOR1, int COLOR2)
{
  #ifdef COMMON_ANODE
    digitalWrite(COLOR1, HIGH);
    digitalWrite(COLOR2, HIGH);
  #endif
  #ifdef COMMON_CATHODE
    digitalWrite(COLOR1, LOW);
    digitalWrite(COLOR2, LOW);
  #endif
}
void LED_led_OFF(int COLOR1, int COLOR2, int COLOR3)
{
  #ifdef COMMON_ANODE
    digitalWrite(COLOR1, HIGH);
    digitalWrite(COLOR2, HIGH);
    digitalWrite(COLOR3, HIGH);
  #endif
  #ifdef COMMON_CATHODE
    digitalWrite(COLOR1, LOW);
    digitalWrite(COLOR2, LOW);
    digitalWrite(COLOR3, LOW);
  #endif
}

void LED_blink(int COLOR1, int number, int delay_on, int delay_off)
{
  for (int j = 1; j <= number; j++) {
    #ifdef COMMON_ANODE
      digitalWrite(COLOR1, LOW);
    #endif
    #if defined(LED_STATUS_PIN) || defined(COMMON_CATHODE)
      digitalWrite(COLOR1, HIGH);
    #endif
    delay(delay_on);
    #ifdef COMMON_ANODE
      digitalWrite(COLOR1, HIGH);
    #endif
    #if defined(LED_STATUS_PIN) || defined(COMMON_CATHODE)
      digitalWrite(COLOR1, LOW);
    #endif
    delay(delay_off);
  }
}
void LED_blink(int COLOR1, int COLOR2, int number, int delay_on, int delay_off)
{
  for (int j = 1; j <= number; j++) {
    #ifdef COMMON_ANODE
      digitalWrite(COLOR1, LOW);
      digitalWrite(COLOR2, LOW);
    #endif
    #ifdef COMMON_CATHODE
      digitalWrite(COLOR1, HIGH);
      digitalWrite(COLOR2, HIGH);
    #endif
    delay(delay_on);
    #ifdef COMMON_ANODE
      digitalWrite(COLOR1, HIGH);
      digitalWrite(COLOR2, HIGH);
    #endif
    #ifdef COMMON_CATHODE
      digitalWrite(COLOR1, LOW);
      digitalWrite(COLOR2, LOW);
    #endif
    delay(delay_off);
  }
}
void LED_blink(int COLOR1, int COLOR2, int COLOR3, int number, int delay_on, int delay_off)
{
  for (int j = 1; j <= number; j++) {
    #ifdef COMMON_ANODE
      digitalWrite(COLOR1, LOW);
      digitalWrite(COLOR2, LOW);
      digitalWrite(COLOR3, LOW);
    #endif
    #ifdef COMMON_CATHODE
      digitalWrite(COLOR1, HIGH);
      digitalWrite(COLOR2, HIGH);
      digitalWrite(COLOR3, HIGH);
    #endif
    delay(delay_on);
    #ifdef COMMON_ANODE
      digitalWrite(COLOR1, HIGH);
      digitalWrite(COLOR2, HIGH);
      digitalWrite(COLOR3, HIGH);
    #endif
    #ifdef COMMON_CATHODE
      digitalWrite(COLOR1, LOW);
      digitalWrite(COLOR2, LOW);
      digitalWrite(COLOR3, LOW);
    #endif
    delay(delay_off);
  }
}

void LED_init()
{
#if defined(COMMON_ANODE) || defined(COMMON_CATHODE)
  pinMode(LED_STATUS_RED, OUTPUT);
  pinMode(LED_STATUS_GREEN, OUTPUT);
  pinMode(LED_STATUS_BLUE, OUTPUT);
#endif
#ifdef LED_STATUS_PIN 
  pinMode(LED_STATUS_PIN, OUTPUT);
#endif

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
#ifdef LED_STATUS_PIN
  digitalWrite(LED_STATUS_PIN, LOW);
#endif
}
