//BathLogic FUNCTIONS .INO FILE
void flow () // Interrupt function
{
  flow_frequency++;
}

void bathWaitTimerReached(MillisTimer &mt) {

  waiting = false;
  waitingTime = 0;
  digitalWrite(rele, LOW);
  
  // TODO - Compute the bath statistics
  DBG_OUTPUT_PORT.println("Bath waiting time reached! Now shower is enabled!");
  waitingTime = waitingTime + stoppedTime;
  flow_frequency = 0;
  bathTime = 0;
  stoppedTime = 0;

  //Reseting the timers
  bathDurationTimer.reset();
  bathStopTimer.reset();
  bathWaitingTimer.reset();
}

void bathTimeReached(MillisTimer &mt) {
  //The bathTime was reached, turnoff the shower
  bathRunning = false;
  waiting = true;
  digitalWrite(rele, HIGH);
  showerIsOn = false;

  bathWaitingTimer.setInterval(EEPROM.read(address_pausa) * 1000);
  bathWaitingTimer.expiredHandler(bathWaitTimerReached);
  bathWaitingTimer.start();
  DBG_OUTPUT_PORT.println("Bath time reached! Triggering the wait time of: " + (String)EEPROM.read(address_pausa));

}

void bathStoppedTimerReached(MillisTimer &mt) {
  bathRunning = false;
  waiting = true;
  digitalWrite(rele, HIGH);
  showerIsOn = false;
  bathWaitingTimer.setInterval(EEPROM.read(address_pausa) * 1000);
  bathWaitingTimer.expiredHandler(bathWaitTimerReached);
  bathWaitingTimer.start();
  DBG_OUTPUT_PORT.println("Bath stop reached! Triggering the wait time of: " + (String)EEPROM.read(address_pausa));

}

void initBathConfiguration() {
  pinMode(FLOW_SENSOR_PIN, INPUT);
  //digitalWrite(FLOW_SENSOR_PIN, HIGH); // Optional Internal Pull-Up
  attachInterrupt(FLOW_SENSOR_PIN, flow, RISING); // Setup Interrupt
  sei(); // Enable interrupts
  currentTime = millis();
  cloopTime = currentTime;
  waiting = false;
  bathRunning = false;
  showerIsOn = false;
  flow_frequency = 0;
  bathTime = 0;
  stoppedTime = 0;
  waitingTime = 0;
  l_hour = 0;
}

void computeBathWorking() {
  if (currentTime >= (cloopTime + 1000)) {
    bathTime++;
    // Pulse frequency (Hz) = 7.5Q, Q is flow rate in L/min.
    l_hour = (flow_frequency * 60 / 7.5); // (Pulse frequency x 60 min) / 7.5Q = flowrate in L/hour
    flow_frequency = 0; // Reset Counter
    cloopTime = currentTime;
  }
}
void bathProcess ()
{ 
  // TEST - Test if the variable currentTime will be good for all bathTime/waitingTime/stoppedTime
  currentTime = millis();
  if (waiting != true) {
    // We might have to modify it because of the pipe drain
    if (flow_frequency > 0) {
      showerIsOn = true;
      bathDurationTimer.run();
      // Indicates that a new bath has started
      if (bathRunning == false && !bathDurationTimer.isRunning()) {
        DBG_OUTPUT_PORT.println("Initializing a new bath with max time: " + (String)EEPROM.read(address_tempo));
        bathDurationTimer.setInterval(EEPROM.read(address_tempo) * 1000);
        bathDurationTimer.expiredHandler(bathTimeReached);
        bathDurationTimer.start();
        bathRunning = true;

        DBG_OUTPUT_PORT.println("The bath stopped time is default set as 1 minutes");
        // TODO - CHANGE TO 60 * 1000
        bathStopTimer.setInterval(10 * 1000);
        bathStopTimer.expiredHandler(bathStoppedTimerReached);
        bathStopTimer.start();
      }

    } else if (bathRunning == true) {
      showerIsOn = false;
      bathStopTimer.run();
    }
    // TODO - Implement the alert LED that the bath is finishing
  } else {
    bathWaitingTimer.run();
  }
}

