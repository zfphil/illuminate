/*
  Copyright (c) 2018, Zachary Phillips (UC Berkeley)
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
      Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
      Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
      Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL ZACHARY PHILLIPS (UC BERKELEY) BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "ledarray.h"

/* A function to print current LED positions (xyz) */
void LedArray::printLedPositions(bool print_na)
{
  // TODO: Format this like a json
  int16_t led_number;
  float na_x, na_y, x, y, z;

  if (print_na)
  {
    buildNaList(led_array_distance_z);
    Serial.println(F("{\n    \"led_position_list_na\" = {"));
  }
  else
    Serial.println(F("{\n    \"led_position_list_cartesian\" : {"));

  for (uint16_t led_index = 0; led_index < led_array_interface->led_count; led_index++)
  {
    led_number = (int16_t)pgm_read_word(&(ledMap[led_index][0]));

    if (print_na)
    {
      na_x = led_position_list_na[led_number][0];
      na_y = led_position_list_na[led_number][1];

      Serial.printf(F("        \"%d\" : ["), led_number);
      Serial.printf(F("%01.03f, "), na_x);
      if (led_index != led_array_interface->led_count - 1)
        Serial.printf(F("%01.03f],\n"), na_y);
      else
        Serial.printf(F("%01.03f]\n"), na_y);
    }
    else
    {
      x = float((int16_t)pgm_read_word(&(ledMap[led_index][2]))) / 100.0;
      y = float((int16_t)pgm_read_word(&(ledMap[led_index][3]))) / 100.0;
      z = float((int16_t)pgm_read_word(&(ledMap[led_index][4]))) / 100.0;
      z = z - float((int16_t)pgm_read_word(&(ledMap[0][4]))) / 100.0 + led_array_distance_z;

      Serial.printf(F("        \"%d\" : ["), led_number);
      Serial.printf(F("%02.02f, "), x);
      Serial.printf(F("%02.02f, "), y);
      if (led_index != led_array_interface->led_count - 1)
        Serial.printf(F("%02.02f],\n"), z);
      else
        Serial.printf(F("%02.02f]\n"), z);
    }
  }
  Serial.println(F("    }\n}"));
}

/* A function to print current LED values */
void LedArray::printCurrentLedValues()
{
  uint16_t led_number;
  Serial.println(F("{\n    \"led_values\" : {"));
  for (uint16_t led_index = 0; led_index < led_array_interface->led_count; led_index++)
  {
    led_number = (int16_t)pgm_read_word(&(ledMap[led_index][0]));

    Serial.printf(F("        \"%d\" : ["), led_number);
    for (int color_channel_index = 0; color_channel_index < led_array_interface->color_channel_count; color_channel_index++)
    {
      if (led_array_interface->bit_depth == 16)
        Serial.printf(F("%lu"), led_array_interface->getLedValue(led_number, color_channel_index));
      else if (led_array_interface->bit_depth == 8)
        Serial.printf(F("%u"), led_array_interface->getLedValue(led_number, color_channel_index));
      else if (led_array_interface->bit_depth == 1)
        Serial.printf(F("%u"), led_array_interface->getLedValue(led_number, color_channel_index));

      if (color_channel_index < (led_array_interface->color_channel_count - 1))
        Serial.print(',');
      else
      {
        if (led_index !=  led_array_interface->led_count - 1)
          Serial.print(F("], \n"));
        else
          Serial.print(F("] \n"));
      }
    }
  }
  Serial.println("    }\n}");
}

/* A function to the version of this device */
void LedArray::printVersion()
{
  Serial.println(version);
}

/* A function to print a human-readable about page */
void LedArray::printAbout()
{
  Serial.print("=== ");
  Serial.print(led_array_interface->device_name);
  Serial.println(F(" LED Array Controller"));
  Serial.print(F("=== HW Version: r"));
  Serial.print(led_array_interface->device_hardware_revision);
  Serial.print(F(", Controller Version: r"));
  Serial.print(version);
  Serial.println(F(")\n=== For help, type ? "));
}

/* A function to print a json-formatted file which contains relevant system parameters */
void LedArray::printSystemParams()
{
  Serial.println(F("{"));
  Serial.print(F("    \"device_name\" : \""));
  Serial.print(led_array_interface->device_name);
  Serial.print(F("\",\n    \"led_count\" : "));
  Serial.print(led_array_interface->led_count);
  Serial.print(F(",\n    \"color_channels\" : ["));
  for (int channel_index = 0; channel_index < led_array_interface->color_channel_count; channel_index++)
  {
    if (channel_index > 0)
      Serial.print(F(", "));
    Serial.print('\"');
    Serial.print(led_array_interface->color_channel_names[channel_index]);
    Serial.print('\"');
  }
  Serial.print(F("]"));
  Serial.print(F(",\n    \"color_channel_center_wavelengths\" : {"));
  for (int channel_index = 0; channel_index < led_array_interface->color_channel_count; channel_index++)
  {
    if (channel_index > 0)
      Serial.print(F(", "));
    Serial.print('\"');
    Serial.print(led_array_interface->color_channel_names[channel_index]);
    Serial.print('\"');
    Serial.printf(" : %.3f", led_array_interface->color_channel_center_wavelengths[channel_index]);
  }
  Serial.print(F("},\n    \"trigger_input_count\" : "));
  Serial.print(led_array_interface->trigger_input_count);
  Serial.print(F(",\n    \"trigger_output_count\" : "));
  Serial.print(led_array_interface->trigger_output_count);
  Serial.print(F(",\n    \"bit_depth\" : "));
  Serial.print(led_array_interface->bit_depth);
  Serial.print(F(",\n    \"serial_number\" : "));
  Serial.print(led_array_interface->serial_number);

  // Terminate JSON
  Serial.println("\n}");
}

/* A function to reset the device to power-on state */
void LedArray::reset()
{
  Serial.println(F("Resetting Array"));
  led_array_interface->clear();
  led_array_interface->deviceSetup();
}

/* A function to draw a random "disco" pattern. For parties, mostly. */
void LedArray::drawDiscoPattern(uint16_t nLed)
{
  if (nLed <= 0)
    nLed = 10;
  while (Serial.available() == 0)
  {
    led_array_interface->setLed(-1, -1, false);
    for (uint16_t led_index = 0; led_index < nLed; led_index++)
    {
      led_index = random(1, led_array_interface->led_count);
      for (int color_channel_index = 0; color_channel_index <  led_array_interface->color_channel_count; color_channel_index++)
        led_array_interface->setLed(led_index, color_channel_index, (uint8_t)random(0, 255));
    }
    led_array_interface->update();
    delay(1);
  }
}

/* A function to clear the calculated NA positions of each LED */
void LedArray::clearNaList()
{
  for ( int16_t led_index = 0; led_index < led_array_interface->led_count; led_index++)
    delete[] led_position_list_na[led_index];
  delete[] led_position_list_na;
}

/* A function to calculate the NA of each LED given the XYZ position and an offset */
void LedArray::buildNaList(float new_board_distance)
{
  float Na_x, Na_y, Na_d, yz, xz, x, y, z;
  // Initialize new position list
  led_position_list_na = new float * [led_array_interface->led_count];

  if (new_board_distance > 0)
    led_array_distance_z = new_board_distance;

  for ( int16_t led_index = 0; led_index < led_array_interface->led_count; led_index++)
  {
    led_position_list_na[led_index] = new float[3];
    if ((int16_t)pgm_read_word(&(ledMap[led_index][1])) >= 0)
    {
      x = float((int16_t)pgm_read_word(&(ledMap[led_index][2]))) / 100.0;
      y = float((int16_t)pgm_read_word(&(ledMap[led_index][3]))) / 100.0;
      z = led_array_distance_z;

      yz = sqrt(y * y + z * z);
      xz = sqrt(x * x + z * z);
      Na_x = sin(atan(x / yz));
      Na_y = sin(atan(y / xz));
      Na_d = sqrt(Na_x * Na_x + Na_y * Na_y);

      led_position_list_na[led_index][0] = Na_x;
      led_position_list_na[led_index][1] = Na_y;
      led_position_list_na[led_index][2] = Na_d;
    }
    else
    {
      led_position_list_na[led_index][0] = INVALID_NA; // invalid NA
      led_position_list_na[led_index][1] = INVALID_NA; // invalid NA
      led_position_list_na[led_index][2] = INVALID_NA; // invalid NA
    }
  }
  if (debug)
    Serial.println(F("Finished updating led positions."));
}

/* A function to fill the LED array with the color specified by led_value */
void LedArray::fillArray()
{
  //TODO: Add check for max current
  drawCircle(0.0, 1.0);
  led_array_interface->update();

  if (debug)
    Serial.println(F("Filled Array"));
}

/* A function to clear the LED array */
void LedArray::clear()
{
  led_array_interface->setLed(-1, -1, (uint8_t)0);
  led_array_interface->update();
}

/* A function to set the numerical aperture of the system*/
void LedArray::setNa(int8_t new_na)
{
  if ((new_na > 0) && new_na < 100 * led_array_interface->max_na)
  {
    objective_na = (float)new_na / 100.0;
    if (debug > 0)
    {
      Serial.print(F("New NA set to: "));
      Serial.println(objective_na);
    }
  } else
    Serial.println(F("ERROR - invalid NA! Make sure NA is 100*na"));
}

void LedArray::printTriggerSettings()
{
  // Input Pins
  for (int trigger_index = 0; trigger_index < TRIGGER_INPUT_COUNT; trigger_index++)
  {
    Serial.print("Trigger input pin index ");
    Serial.print(trigger_index);
    Serial.print(F(" uses Pin #"));
    Serial.println(led_array_interface->trigger_input_pin_list[trigger_index]);
  }

  // Output Pins
  for (int trigger_index = 0; trigger_index < TRIGGER_OUTPUT_COUNT; trigger_index++)
  {
    Serial.print(F("Trigger output pin index "));
    Serial.print(trigger_index);
    Serial.print(F(" uses Pin #"));
    Serial.print(led_array_interface->trigger_output_pin_list[trigger_index]);
    Serial.print(F(" with pulse width "));
    Serial.print(trigger_pulse_width_list_us[trigger_index]);
    Serial.print(F("us. Start delay is "));
    Serial.print(trigger_start_delay_list_us[trigger_index]);
    Serial.println(F("us."));
  }
}

/* A function to draw a DPC navigator pattern */
void LedArray::drawNavDpc()
{
  notImplemented("Navigator DPC");
}

/* A function to draw a darkfield pattern */
void LedArray::drawDarkfield()
{
  //TODO: Add check for max current
  drawCircle(objective_na, 1.0);
  led_array_interface->update();
}

/* A function to draw a cDPC pattern */
void LedArray::drawCdpc(int argc, char * *argv)
{
  if (led_array_interface->color_channel_count != 3)
  {
    notImplemented("cDPC");
    return;
  }
  else
  {
    uint8_t illumination_intensity;

    if (argc == 0)
      illumination_intensity = 127;
    else if (argc == 1)
      illumination_intensity = (uint8_t)atoi(argv[0]);
    else
    {
      Serial.println(F("ERROR (LedArray::drawCdpc): Invalid number of arguments"));
      return;
    }

    // This mask defines the cDPC pattern
    int cdpc_mask[4][3] = {
      {1, 0, 0},
      {1, 0, 1},
      {0, 1, 1},
      {0, 1, 0}
    };

    // Clear array
    led_array_interface->clear();
    for (int quadrant_index = 0; quadrant_index < 4; quadrant_index++)
    {
      // Set all colors to zero (off)
      for (int color_channel_index = 0; color_channel_index < led_array_interface->color_channel_count; color_channel_index++)
        led_value[color_channel_index] = 0;
      for (int color_index = 0; color_index < 3; color_index++)
      {
        if (cdpc_mask[quadrant_index][color_index])
        {
          led_value[color_index] = illumination_intensity;
          drawQuadrant(quadrant_index, 0.0, objective_na, true);
        }
      }
    }
    led_array_interface->update();
  }
}

/* A function to draw a half annulus */
void LedArray::drawHalfAnnulus(int argc, char * *argv)
{
  float na_start, na_end;
  if (argc == 1)
  {
    na_start = objective_na;
    na_end = objective_na + 0.2;
  }
  else if (argc == 3)
  {
    na_start = atof(argv[1]) / 100.0;
    na_end = atof(argv[2]) / 100.0;
  }
  else
  {
    Serial.println(F("ERROR (LedArray::drawHalfAnnulus) Invlaid number of arguments."));
    return;
  }

  if (debug >= 1)
  {
    Serial.print(F("Drawing half-annulus pattern with type: "));
    Serial.print(argv[0]);
    Serial.print(F(" from "));
    Serial.print(na_start);
    Serial.print(F("NA to "));
    Serial.print(na_end);
    Serial.println(F("NA."));
  }

  int8_t half_annulus_type = 0;
  if ( (strcmp(argv[0], DPC_TOP1) == 0) || (strcmp(argv[0], DPC_TOP2) == 0))
    half_annulus_type = 0;
  else if ( (strcmp(argv[0], DPC_BOTTOM1) == 0) || (strcmp(argv[0], DPC_BOTTOM2) == 0))
    half_annulus_type = 1;
  else if ( (strcmp(argv[0], DPC_LEFT1) == 0) || (strcmp(argv[0], DPC_LEFT2) == 0))
    half_annulus_type = 2;
  else if ( (strcmp(argv[0], DPC_RIGHT1) == 0) || (strcmp(argv[0], DPC_RIGHT2) == 0))
    half_annulus_type = 3;
  else
    Serial.println(F("ERROR - invalid half annulus circle type. Options are t, b, l, and r"));

  if (half_annulus_type >= 0)
  {
    drawHalfCircle(half_annulus_type, na_start, na_end);
    led_array_interface->update();
  }
}

/* A function to draw a color darkfield pattern */
void LedArray::drawColorDarkfield(int argc, char * * argv)
{
  if (led_array_interface->color_channel_count != 3)
  {
    notImplemented("Color darkfield");
    return;
  }
  else
  {
    uint8_t illumination_intensity = 127;
    float start_na = objective_na;
    float end_na = objective_na + 0.2;

    if (argc == 0)
      ; // do nothing (use default values above)
    else if (argc >= 1)
      illumination_intensity = (uint8_t)atoi(argv[0]);
    else if (argc >= 2)
    {
      start_na = atof(argv[1]);
      end_na = min(start_na + 0.2, 1.0);
    }
    else if (argc >= 3)
    {
      start_na = atof(argv[1]);
      start_na = atof(argv[2]);
    }
    else
    {
      Serial.println(F("ERROR (LedArray::drawColorDarkfield): Invalid number of arguments"));
      return;
    }

    // This mask defines the cDPC pattern
    int cdf_mask[4][3] = {
      {1, 0, 0},
      {1, 0, 1},
      {0, 1, 1},
      {0, 1, 0}
    };

    // Clear array
    led_array_interface->clear();
    for (int quadrant_index = 0; quadrant_index < 4; quadrant_index++)
    {
      // Set all colors to zero (off)
      for (int color_channel_index = 0; color_channel_index < led_array_interface->color_channel_count; color_channel_index++)
        led_value[color_channel_index] = 0;
      for (int color_index = 0; color_index < 3; color_index++)
      {
        if (cdf_mask[quadrant_index][color_index])
        {
          led_value[color_index] = illumination_intensity;
          drawQuadrant(quadrant_index, start_na, end_na, true);
        }
      }
    }
    led_array_interface->update();
  }
}

/* A function to draw an annulus*/
void LedArray::drawAnnulus(int argc, char * * argv)
{
  float start_na, end_na;
  if (argc == 0)
  {
    start_na = objective_na;
    end_na = min(objective_na + 0.2, 1.0);
  }
  else if (argc == 2)
  {
    start_na = atof(argv[0]);
    end_na = atof(argv[1]);
  }
  else
  {
    Serial.println(F("ERROR (LedArray::drawAnnulus): Invalid number of arguments!"));
    return;
  }

  if (debug >= 1)
  {
    Serial.print(F("Drawing annulus from "));
    Serial.print(start_na);
    Serial.print(F("NA to "));
    Serial.print(end_na);
    Serial.println(F("NA."));
  }

  if (auto_clear_flag)
    clear();

  // Draw circle
  drawCircle(start_na, end_na);
  led_array_interface->update();
}

/* A function to draw a spoecific LED channel as indexed in hardware */
void LedArray::drawChannel(int argc, char * *argv)
{
  if (argc != 1)
    Serial.print(F("ERROR (LedArray::drawChannel): invalid argument count"));
  else
  {
    if (auto_clear_flag)
      clear();
      
    for (int color_channel_index = 0; color_channel_index < led_array_interface->color_channel_count; color_channel_index++)
      led_array_interface->setChannel(strtol(argv[0], NULL, 0), color_channel_index, led_value[color_channel_index]);
      led_array_interface->update();
  }
}

/* A function to set the pin order of a LED (for multi-color designs */
void LedArray::setPinOrder(int argc, char * *argv)
{
  notImplemented("Set Pin Order");
}

/* Trigger setup function for setting the trigger pulse width and delay after sending */
void LedArray::triggerSetup(int argc, char ** argv)
{
  int trigger_index = 0;
  uint16_t trigger_pulse_width_us = 0;
  uint16_t trigger_start_delay_ms = 0;

  if (argc >= 2)
  {
    trigger_index = atoi(argv[0]);
    trigger_pulse_width_us = strtoul(argv[1], NULL, 0);
    if (argc >= 3)
      trigger_start_delay_ms = strtoul(argv[2], NULL, 0);

    if (trigger_pulse_width_us >= 0)
      trigger_pulse_width_list_us[trigger_index] = trigger_pulse_width_us;

    if (trigger_start_delay_ms >= 0)
      trigger_start_delay_list_us[trigger_index] = trigger_start_delay_ms;

    Serial.print("Trigger ");
    Serial.print(trigger_index);
    Serial.print(" now has a pulse width of ");
    Serial.print(trigger_pulse_width_list_us[trigger_index] );
    Serial.print("us and a start delay of ");
    Serial.print(trigger_start_delay_list_us[trigger_index]);
    Serial.println(F("us."));
  }
  else
    Serial.println(F("ERROR: Invalid number of arguments for setTriggerPulse!"));
}

/* Send a trigger pulse */
void LedArray::sendTriggerPulse(int trigger_index, bool show_output)
{
  // TODO: store polarity and use it here

  // Send trigger pulse with pulse_width
  int status = led_array_interface->sendTriggerPulse(trigger_index, trigger_pulse_width_list_us[trigger_index], true);

  if (status < 0)
    Serial.print(F("ERROR - pin not configured!"));
}

void LedArray::setTriggerState(int trigger_index, bool state, bool show_output)
{
  int status = led_array_interface->setTriggerState(trigger_index, state);
  if (status < 0)
    Serial.print(F("ERROR - pin not configured!"));
}

bool LedArray::getTriggerState(int trigger_index)
{
  return (digitalReadFast(led_array_interface->trigger_input_pin_list[trigger_index]));
}

/* Wait for a TTL trigger port to be in the given state */
void LedArray::waitForTriggerState(int trigger_index, bool state)
{
  float delayed_ms = 0;
  while ((digitalReadFast(led_array_interface->trigger_input_pin_list[trigger_index]) != state))
  {
    delayMicroseconds(1);
    delayed_ms += 0.001;
    if (delayed_ms > trigger_feedback_timeout_ms)
    {
      Serial.print(F("WARNING: Exceeding max delay for trigger input "));
      Serial.println(trigger_index);
      led_array_interface->setLed(-1, -1, (uint8_t)0);
      break;
    }
  }
}

/* A trigger test function */
void LedArray::triggerInputTest(uint16_t channel)
{
  led_array_interface->setLed(-1, -1, (uint8_t)0);
  led_array_interface->update();
  Serial.println(led_array_interface->trigger_input_state[channel]);
  Serial.print("Begin trigger input test for channel "); Serial.println(channel);
  waitForTriggerState(channel, !led_array_interface->trigger_input_state[channel]);
  Serial.print("Passed trigger input test for channel "); Serial.println(channel);
  led_array_interface->setLed(-1, -1, (uint8_t)0);
  led_array_interface->setLed(0, -1, (uint8_t)255);
  led_array_interface->update();
}

/* Draw a LED list */
void LedArray::drawLedList(uint16_t argc, char ** argv)
{
  uint16_t led_number;
  if (auto_clear_flag)
    clear();
  for (uint16_t led_index = 0; led_index < argc; led_index++)
  {
    led_number = strtoul(argv[led_index], NULL, 0);
    for (int color_channel_index = 0; color_channel_index < led_array_interface->color_channel_count; color_channel_index++)
      led_array_interface->setLed(led_number, color_channel_index, led_value[color_channel_index]);
  }
  led_array_interface->update();
}

/* Scan brightfield LEDs */
void LedArray::scanBrightfieldLeds(uint16_t argc, char ** argv)
{
  uint16_t delay_ms = 0;

  // Parse inputs
  if (argc == 1)
    delay_ms = strtoul(argv[0], NULL, 0);
  else if (argc <= TRIGGER_OUTPUT_COUNT)
  {
    for (int arg_index = 0; arg_index < argc; arg_index++)
      trigger_output_mode_list[arg_index] = atoi(argv[arg_index]);
  }

  // Scan the LEDs
  scanLedRange(delay_ms, 0.0, objective_na, true);

  if (debug >= 1)
    Serial.println(F("Finished brightfield LED scan"));
}

/* Scan all LEDs */
void LedArray::scanAllLeds(uint16_t argc, char ** argv)
{
  uint16_t delay_ms = 0;
  if (argc == 1)
    delay_ms = strtoul(argv[0], NULL, 0);

  if ((delay_ms >= 0) && (delay_ms < DELAY_MAX))
  {
    // Clear array initially
    clear();
    for (int trigger_index = 0; trigger_index < TRIGGER_OUTPUT_COUNT; trigger_index++)
    {
      if (trigger_output_mode_list[trigger_index] == TRIG_MODE_START)
        sendTriggerPulse(trigger_index, false);
    }

    // Initiate LED scan
    scanLedRange(delay_ms, 0.0, 1.0, true);

    if (debug >= 1)
      Serial.println(F("Finished full LED scan."));
  }
  else
    Serial.println(F("ERROR - full scan delay too short/long"));
}

/* Allows setting of current color buffer, which is respected by most other commands */
void LedArray::setColor(int16_t argc, char ** argv)
{
  // TODO: check if argv is within valid range
  // TODO: respect bit_depth
  if (argc == 0)
    ; // Do nothing
  else if (argc == 1)
  {
    if (strcmp(argv[0], "red") == 0 && led_array_interface->color_channel_count == 3)
    {
      led_value[0] = default_brightness;
      led_value[1] = 0;
      led_value[2] = 0;
    }
    else if (strcmp(argv[0], "green") == 0 && led_array_interface->color_channel_count == 3)
    {
      led_value[0] = 0;
      led_value[1] = default_brightness;
      led_value[2] = 0;
    }
    else if (strcmp(argv[0], "blue") == 0 && led_array_interface->color_channel_count == 3)
    {
      led_value[0] = 0;
      led_value[1] = 0;
      led_value[2] = default_brightness;
    }
    else if (strcmp(argv[0], "redmax") == 0 && led_array_interface->color_channel_count == 3)
    {
      led_value[0] = UINT8_MAX;
      led_value[1] = 0;
      led_value[2] = 0;
    }
    else if (strcmp(argv[0], "greenmax") == 0 && led_array_interface->color_channel_count == 3)
    {
      led_value[0] = 0;
      led_value[1] = UINT8_MAX;
      led_value[2] = 0;
    }
    else if (strcmp(argv[0], "bluemax") == 0 && led_array_interface->color_channel_count == 3)
    {
      led_value[0] = 0;
      led_value[1] = 0;
      led_value[2] = UINT8_MAX;
    }
    else if ((strcmp(argv[0], "all") == 0) || (strcmp(argv[0], "white") == 0))
    {
      for (int color_channel_index = 0; color_channel_index < led_array_interface->color_channel_count; color_channel_index++)
        led_value[color_channel_index] = default_brightness;

    }
    else if (strcmp(argv[0], "max") == 0)
    {
      for (int color_channel_index = 0; color_channel_index < led_array_interface->color_channel_count; color_channel_index++)
        led_value[color_channel_index] = UINT8_MAX;
    }
    else if (strcmp(argv[0], "half") == 0)
    {
      for (int color_channel_index = 0; color_channel_index < led_array_interface->color_channel_count; color_channel_index++)
        led_value[color_channel_index] = (uint8_t) ((float)UINT8_MAX / 2);
    }
    else if (strcmp(argv[0], "quarter") == 0)
    {
      for (int color_channel_index = 0; color_channel_index < led_array_interface->color_channel_count; color_channel_index++)
        led_value[color_channel_index] = (uint8_t) ((float)UINT8_MAX / 4);
    }
    else
    {
      for (int color_channel_index = 0; color_channel_index < led_array_interface->color_channel_count; color_channel_index++)
        led_value[color_channel_index] = (uint8_t)atoi(argv[0]);
    }
  }
  else if (argc == led_array_interface->color_channel_count)
  {
    for (int color_channel_index = 0; color_channel_index < led_array_interface->color_channel_count; color_channel_index++)
      led_value[color_channel_index] = (uint8_t)atoi(argv[color_channel_index]);
  }
  else
  {
    Serial.println(F("ERROR (LedArray::setColor): Invalid number of color values"));
    return;
  }

  // Print current colors regardless of input
  Serial.print(F("Current colors: "));
  for (int color_channel_index = 0; color_channel_index < led_array_interface->color_channel_count; color_channel_index++)
  {
    Serial.print(led_value[color_channel_index]);
    if (color_channel_index < (led_array_interface->color_channel_count - 1))
      Serial.print(',');
  }
  Serial.print('\n');
}

/* Draws a single quadrant of LEDs using standard quadrant indexing (top left is 0, moving clockwise) */
void LedArray::drawQuadrant(int quadrant_number, float start_na, float end_na, bool include_center)
{
  if (debug >= 2)
  {
    Serial.print(F("Drawing Quadrant "));
    Serial.println(quadrant_number);
  }

  for ( int16_t led_index = 0; led_index < led_array_interface->led_count; led_index++)
  {
    float x = led_position_list_na[led_index][0];
    float y = led_position_list_na[led_index][1];
    float d = led_position_list_na[led_index][2];

    if (!include_center)
    {
      if (  (quadrant_number == 0 && (x < 0) && (y > 0) && (d <= end_na) && (d >= start_na))
            || (quadrant_number == 1 && (x > 0) && (y > 0) && (d <= end_na) && (d >= start_na))
            || (quadrant_number == 2 && (x > 0) && (y < 0) && (d <= end_na) && (d >= start_na))
            || (quadrant_number == 3 && (x < 0) && (y < 0) && (d <= end_na) && (d >= start_na)))
      {
        for (int color_channel_index = 0; color_channel_index < led_array_interface->color_channel_count; color_channel_index++)
          led_array_interface->setLed(led_index, color_channel_index, led_value[color_channel_index]);
      }
    }
    else
    {
      if (  (quadrant_number == 0 && (x <= 0) && (y >= 0) && (d <= end_na) && (d >= start_na))
            || (quadrant_number == 1 && (x >= 0) && (y >= 0) && (d <= end_na) && (d >= start_na))
            || (quadrant_number == 2 && (x >= 0) && (y <= 0) && (d <= end_na) && (d >= start_na))
            || (quadrant_number == 3 && (x <= 0) && (y <= 0) && (d <= end_na) && (d >= start_na)))
      {
        for (int color_channel_index = 0; color_channel_index < led_array_interface->color_channel_count; color_channel_index++)
          led_array_interface->setLed(led_index, color_channel_index, led_value[color_channel_index]);
      }
    }
  }
}

/* Draws a single half-circle of LEDs using standard quadrant indexing (top left is 0, moving clockwise) */
void LedArray::drawHalfCircle(int8_t half_circle_type, float start_na, float end_na)
{
  if (debug >= 2)
  {
    Serial.println(F("Drawing Half Annulus:"));
    Serial.println(half_circle_type);
  }

  float x, y, d;
  for ( int16_t led_index = 0; led_index < led_array_interface->led_count; led_index++)
  {
    x = led_position_list_na[led_index][0];
    y = led_position_list_na[led_index][1];
    d = led_position_list_na[led_index][2];

    if (d > (start_na) && (d <= (end_na)))
    {
      if (  (half_circle_type == 0 && (y > 0))      // Top
            || (half_circle_type == 1 && (y < 0))   // Bottom
            || (half_circle_type == 2 && (x < 0))   // Left
            || (half_circle_type == 3 && (x > 0)))  // Right

        for (int color_channel_index = 0; color_channel_index < led_array_interface->color_channel_count; color_channel_index++)
          led_array_interface->setLed(led_index, color_channel_index, led_value[color_channel_index]);
    }
  }
}

/* Draws a circle or annulus of LEDs */
void LedArray::drawCircle(float start_na, float end_na)
{
  if (debug >= 2)
  {
    Serial.print(F("Drawing circle from "));
    Serial.print(start_na);
    Serial.print(F("NA to "));
    Serial.print(end_na);
    Serial.println(F("NA"));
  }

  float d;
  for ( int16_t led_index = 0; led_index < led_array_interface->led_count; led_index++)
  {
    d = led_position_list_na[led_index][2];
    if (d >= (start_na) && (d <= (end_na)))
    {
      for (int color_channel_index = 0; color_channel_index < led_array_interface->color_channel_count; color_channel_index++)
        led_array_interface->setLed(led_index, color_channel_index, led_value[color_channel_index]);
    }
  }
}

/* Scan brightfield LEDs */
void LedArray::scanLedRange(uint16_t delay_ms, float start_na, float end_na, bool print_indicies)
{
  float d;

  for (int trigger_index = 0; trigger_index < TRIGGER_OUTPUT_COUNT; trigger_index++)
  {
    if (trigger_output_mode_list[trigger_index] == TRIG_MODE_START)
      sendTriggerPulse(trigger_index, false);
  }

  uint16_t led_index = 0;
  while (led_index < led_array_interface->led_count)
  {
    d = led_position_list_na[led_index][2];

    if (d >= start_na && d <= end_na)
    {
      // Clear all LEDs
      led_array_interface->clear();

      // Set LEDs
      for (int color_channel_index = 0; color_channel_index < led_array_interface->color_channel_count; color_channel_index++)
        led_array_interface->setLed(led_index, color_channel_index, led_value[color_channel_index]);

      if (print_indicies)
      {
        Serial.print(led_index);
        if (led_index < led_array_interface->led_count - 1)
          Serial.print(", ");
        else
          Serial.print('\n');
      }

      // Update LED Pattern
      led_array_interface->update();

      // Send trigger pulse
      for (int trigger_index = 0; trigger_index < TRIGGER_OUTPUT_COUNT; trigger_index++)
      {
        if (trigger_output_mode_list[trigger_index] == TRIG_MODE_ITERATION)
          sendTriggerPulse(trigger_index, false);
      }

      // Delay for desired wait period
      delay(delay_ms);
    }
    led_index++;
  }
  delay(delay_ms);
  led_array_interface->clear();
}

/* Command parser for DPC */
void LedArray::drawDpc(uint16_t argc, char ** argv)
{
  if (argc == 1)
  {
    if (debug >= 1)
    {
      Serial.print(F("Drew DPC pattern with type: "));
      Serial.println(argv[0]);
    }

    if (auto_clear_flag)
      led_array_interface->clear();

    int8_t dpc_type = -1;
    if ( (strcmp(argv[0], DPC_TOP1) == 0) || (strcmp(argv[0], DPC_TOP2) == 0))
      dpc_type = 0;
    else if ( (strcmp(argv[0], DPC_BOTTOM1) == 0) || (strcmp(argv[0], DPC_BOTTOM2) == 0))
      dpc_type = 1;
    else if ( (strcmp(argv[0], DPC_LEFT1) == 0) || (strcmp(argv[0], DPC_LEFT2) == 0))
      dpc_type = 2;
    else if ( (strcmp(argv[0], DPC_RIGHT1) == 0) || (strcmp(argv[0], DPC_RIGHT2) == 0))
      dpc_type = 3;
    else
      Serial.println(F("ERROR - invalid dpc circle type. Options are dpc.t, dpc.b, dpc.l, dpc.r"));

    if (dpc_type >= 0)
    {
      drawHalfCircle(dpc_type, 0.0, objective_na);
      led_array_interface->update();
    }
  }
  else
    Serial.println(F("ERROR (LedArray::drawDpc) Invlaid number of arguments."));
}

/* Draw brightfield pattern */
void LedArray::drawBrightfield(uint16_t argc, char ** argv)
{
  if (debug)
    Serial.println(F("Drawing brightfield pattern"));

  if (auto_clear_flag)
    clear();

  // Draw circle
  drawCircle(0.0, objective_na);
  led_array_interface->update();
}

/* Set sequence length */
void LedArray::setSequenceLength(uint16_t new_seq_length, bool quiet)
{
  // Reset old sequence
  led_sequence.deallocate();

  // Initalize new sequence
  led_sequence.allocate(new_seq_length);

  if (!quiet)
  {
    Serial.print(F("New sequence length is: "));
    Serial.println(new_seq_length);
  }
}

void LedArray::setSequenceBitDepth(uint8_t bit_depth, bool quiet)
{
  led_sequence.setBitDepth(bit_depth);

  if (!quiet)
  {
    Serial.print(F("New sequence bit depth is: "));
    Serial.println(bit_depth);
  }
}

int LedArray::getSequenceBitDepth()
{
  return led_sequence.bit_depth;
}

/* Set sequence value */
void LedArray::setSequenceValue(uint16_t argc, void ** led_values, int16_t * led_numbers)
{
  // Determine the number of LEDs in this pattern
  int16_t pattern_led_count = 0;
  if (led_numbers[1] == -1)
    pattern_led_count = led_array_interface->led_count;
  else if (led_numbers[1] == -2)
    pattern_led_count = 0;
  else
    pattern_led_count = led_numbers[0];

  // Determine number of arguments to process
  int16_t led_argc = led_numbers[0];

  if (led_argc > 0 && (argc == (led_argc * led_array_interface->color_channel_count))) // Color (or white if one channel)
  {
    // Switch to new led pattern
    if (led_sequence.incriment(pattern_led_count) && (pattern_led_count > 0))
    {
      // Assign LED indicies and values
      uint8_t * values = ((uint8_t *) led_values);
      for (int led_argument_index = 0; led_argument_index < led_argc; led_argument_index++)
      {
        // If the led number is -1, append all LEDs to the sequence
        if (led_numbers[led_argument_index + 1] == -1)
        {
          for (int led_number = 0; led_number < led_array_interface->led_count; led_number++)
          {
            if (led_sequence.bit_depth == 8)
              led_sequence.append(led_number, values[led_argument_index * led_array_interface->color_channel_count]);
            else if (led_sequence.bit_depth == 1)
              led_sequence.append(led_number, true);
          }
        }
        else // Normal LED value
        {
          // Append this LED to the sequence
          if (led_sequence.bit_depth == 8)
            led_sequence.append(led_numbers[led_argument_index + 1], values[led_argument_index * led_array_interface->color_channel_count]);
          else if (led_sequence.bit_depth == 1)
            led_sequence.append(led_numbers[led_argument_index + 1], true);
        }
      }
      if (debug >= 0)
        led_sequence.print(led_sequence.number_of_patterns_assigned - 1);
    }
  }
  else if (pattern_led_count == 0)
  {
    // Add blank pattern
    led_sequence.incriment(0);
  }
  else
  {
    Serial.print(F("Error (LedArray::setSequenceValue) - invalid number of arguments (should be divisible by ")); Serial.print(led_array_interface->color_channel_count); Serial.println(F(")"));
  }
}

void LedArray::printSequence()
{
  Serial.print(F("Sequence has ")); Serial.print(led_sequence.length); Serial.print("x "); Serial.print(led_sequence.bit_depth); Serial.println(F(" bit patterns:"));
  led_sequence.print();
}

void LedArray::printSequenceLength()
{
  Serial.println(led_sequence.length);
}


/* Reset stored sequence */
void LedArray::resetSequence()
{
  sequence_number_displayed = 0;
}

void LedArray::runSequence(uint16_t argc, char ** argv)
{
  if (debug)
    Serial.println(F("Starting sequence."));

  /* Format for argv:
     0: delay between acquisitions, us/ms
     1: number of times to repeat pattern
     2: trigger output 1 setting
     3: trigger output 2 setting
     4: trigger input 1 setting
     5: trigger input 2 setting
  */

  // Reset Trigger parameters
  trigger_output_mode_list[0] = 0;
  trigger_output_mode_list[1] = 0;
  trigger_input_mode_list[0] = 0;
  trigger_input_mode_list[1] = 0;

  uint16_t delay_ms = 10;
  uint16_t acquisition_count = 1;

  for (int argc_index = 0; argc_index < argc; argc_index++)
  {
    if (argc_index == 0)
      delay_ms  = strtoul(argv[0], NULL, 0);
    else if (argc_index == 1)
      acquisition_count  = strtoul(argv[1], NULL, 0);
    else if (argc_index >= 2 && argc_index < 4)
      trigger_output_mode_list[argc_index - 2] = atoi(argv[argc_index]);
    else if (argc_index >= 4 && argc_index < 6)
      trigger_input_mode_list[argc_index - 4] = atoi(argv[argc_index]);
    else
      Serial.println("WARNING:  Ignoring additional argument in runSequence");
  }

  if (debug)
  {
    Serial.println("OPTIONS:");
    Serial.print("  delay: ");
    Serial.print(delay_ms);
    Serial.print("ms\n  acquisition_count: ");
    Serial.println(acquisition_count);
    Serial.print("  trigger out 0: ");
    Serial.println(trigger_output_mode_list[0]);
    Serial.print("  trigger out 1: ");
    Serial.println(trigger_output_mode_list[1]);
    Serial.print("  trigger in 0: ");
    Serial.println(trigger_input_mode_list[0]);
    Serial.print("  trigger in 1: ");
    Serial.println(trigger_input_mode_list[1]);
  }

  // Check to be sure we're not trying to go faster than the hardware will allow
  if ((delay_ms < MIN_SEQUENCE_DELAY))
  {
    Serial.print("ERROR: Sequance delay (");
    Serial.print(delay_ms);
    Serial.print("ms) was shorter than MIN_SEQUENCE_DELAY (");
    Serial.print(MIN_SEQUENCE_DELAY);
    Serial.print("ms).");
    return;
  }

  // Clear LED Array
  led_array_interface->setLed(-1, -1, false);
  led_array_interface->update();

  // Initialize variables
  uint16_t led_number;

  elapsedMicros elapsed_us_outer;

  for (uint16_t acquisition_index = 0; acquisition_index < acquisition_count; acquisition_index++)
  {
    for (uint16_t pattern_index = 0; pattern_index < led_sequence.number_of_patterns_assigned; pattern_index++)
    {
      // Return if we send any command to interrupt.
      if (Serial.available())
        return;

      // Sent output trigger pulses before illuminating
      for (int trigger_index = 0; trigger_index < TRIGGER_OUTPUT_COUNT; trigger_index++)
      {
        if (((trigger_output_mode_list[trigger_index] > 0) && (pattern_index % trigger_output_mode_list[trigger_index] == 0))
            || ((trigger_output_mode_list[trigger_index] == TRIG_MODE_ITERATION) && (pattern_index == 0))
            || ((trigger_output_mode_list[trigger_index] == TRIG_MODE_START) && (acquisition_index == 0 && pattern_index == 0)))
        {
          sendTriggerPulse(trigger_index, false);

          if (trigger_start_delay_list_us[trigger_index] > 0)
            delayMicroseconds(trigger_start_delay_list_us[trigger_index]);
        }
      }

      // Wait for all devices to start acquiring (if input triggers are configured
      for (int trigger_index = 0; trigger_index < TRIGGER_INPUT_COUNT; trigger_index++)
      {
        if (((trigger_input_mode_list[trigger_index] > 0) && (pattern_index % trigger_input_mode_list[trigger_index] == 0))
            || ((trigger_input_mode_list[trigger_index] == TRIG_MODE_ITERATION) && (pattern_index == 0))
            || ((trigger_input_mode_list[trigger_index] == TRIG_MODE_START) && (acquisition_index == 0 && pattern_index == 0)))
          waitForTriggerState(trigger_index, true);
      }

      elapsedMicros elapsed_us_inner;

      // Set all LEDs to zero
      led_array_interface->setLed(-1, -1, false);

      // Define pattern
      for (uint16_t led_idx = 0; led_idx < led_sequence.led_counts[pattern_index]; led_idx++)
      {
        led_number = led_sequence.led_list[pattern_index][led_idx];
        if (led_sequence.bit_depth == 1)
          for (int color_channel_index = 0; color_channel_index < led_array_interface->color_channel_count; color_channel_index++)
            led_array_interface->setLed(led_number, color_channel_index, led_value[color_channel_index]);
        else
          for (int color_channel_index = 0; color_channel_index < led_array_interface->color_channel_count; color_channel_index++)
            led_array_interface->setLed(led_number, color_channel_index, led_sequence.values[pattern_index][led_idx]);
      }

      // Check if led_count is zero - if so, clear the array
      if (led_sequence.led_counts[pattern_index] == 0)
        led_array_interface->clear();

      // Update pattern
      led_array_interface->update();

      // Ensure that we haven't set too short of a delay
      if ((float)elapsed_us_inner > (1000 * (float)delay_ms))
      {
        Serial.println(F("Error - delay too short!"));
        return;
      }

      // Wait for the defined mininum amount of time (delay_ms) before checking trigger input state
      while ((float)elapsed_us_inner < (1000 * (float)delay_ms)) {} // Wait until this is true

      // Wait for all devices to stop acquiring (if input triggers are configured
      for (int trigger_index = 0; trigger_index < TRIGGER_INPUT_COUNT; trigger_index++)
      {
        if (((trigger_input_mode_list[trigger_index] > 0) && (pattern_index % trigger_input_mode_list[trigger_index] == 0))
            || ((trigger_input_mode_list[trigger_index] == TRIG_MODE_ITERATION) && (pattern_index == led_sequence.number_of_patterns_assigned))
            || ((trigger_input_mode_list[trigger_index] == TRIG_MODE_START) && (acquisition_index == acquisition_count && pattern_index == led_sequence.number_of_patterns_assigned)))
          waitForTriggerState(trigger_index, false);
      }
      if (debug)
      {
        Serial.print(F("Elapsed time: "));
        Serial.print((float)elapsed_us_outer);
        Serial.println(F("us"));
      }
    }
  }

  led_array_interface->clear();
  led_array_interface->update();

  // Let user know we're done
  Serial.println("Finished sending sequence.");
}

void LedArray::runSequenceFast(uint16_t argc, char ** argv)
{
  // Check if this LED array supports fast sequencing
  if (!led_array_interface->supports_fast_sequence)
  {
    Serial.println(F("ERROR (LedArray::runSequenceFast): This array does not support fast sequencing!"));
    return;
  }
  
  if (debug)
    Serial.println(F("Starting fast Sequence"));

  uint16_t delay_us = 100;
  uint16_t acquisition_count = 1;
  bool use_ms = false;

  /* Format for argv:
     0: delay between acquisitions, us/ms
     1: whether to use micro-seconds for units (default is false)
     2: number of times to repeat pattern
     3: trigger output 1 setting
     4: trigger output 2 setting
     5: trigger input 1 setting
     6: trigger input 2 setting
  */

  // Reset Trigger parameters
  trigger_output_mode_list[0] = 0;
  trigger_output_mode_list[1] = 0;
  trigger_input_mode_list[0] = 0;
  trigger_input_mode_list[1] = 0;

  for (int argc_index = 0; argc_index < argc; argc_index++)
  {
    if (argc_index == 0)
      delay_us  = strtoul(argv[0], NULL, 0);
    else if (argc_index == 1)
      use_ms = atoi(argv[1]) > 0;
    else if (argc_index == 2)
      acquisition_count  = strtoul(argv[2], NULL, 0);
    else if (argc_index >= 3 && argc_index < 5)
      trigger_output_mode_list[argc_index - 3] = atoi(argv[argc_index]);
    else if (argc_index >= 5 && argc_index < 7)
      trigger_input_mode_list[argc_index - 5] = atoi(argv[argc_index]);
    else
      Serial.println("WARNING:  Ignoring additional argument in runSequence");
  }

  float delay_us_used = 0;
  if (use_ms)
    delay_us_used = 1000.0 * (float)delay_us;
  else
    delay_us_used = (float)delay_us;

  if (debug)
  {
    Serial.println("OPTIONS:");
    Serial.print("  delay_us: ");
    Serial.println(delay_us_used);
    Serial.print("  acquisition_count: ");
    Serial.println(acquisition_count);
    Serial.print("  trigger out 0: ");
    Serial.println(trigger_output_mode_list[0]);
    Serial.print("  trigger out 1: ");
    Serial.println(trigger_output_mode_list[1]);
    Serial.print("  trigger in 0: ");
    Serial.println(trigger_input_mode_list[0]);
    Serial.print("  trigger in 1: ");
    Serial.println(trigger_input_mode_list[1]);
  }

  // Check to be sure we're not trying to go faster than the hardware will allow
  if ((delay_us_used < (float)MIN_SEQUENCE_DELAY_FAST))
  {
    Serial.print("ERROR: Sequance delay (");
    Serial.print(delay_us_used);
    Serial.print("us) was shorter than MIN_SEQUENCE_DELAY_FAST (");
    Serial.print(MIN_SEQUENCE_DELAY_FAST);
    Serial.print("us).");
    return;
  }

  // Clear LED Array
  led_array_interface->setLedFast(-1, -1, 0);

  // Initialize variables
  uint16_t led_number;

  elapsedMicros elapsed_us_outer;
  float elapsed = 0;
  float elapsed_us_0 = 0;
  bool triggered[2];
  float trigger_wait_list[TRIGGER_OUTPUT_COUNT];
  int trigger_finished_waiting_count = 0;

  // Determine max start delay
  uint16_t max_start_delay_us = 0;

  for (uint16_t acquisition_index = 0; acquisition_index < acquisition_count; acquisition_index++)
  {
    for (uint16_t pattern_index = 0; pattern_index < led_sequence.number_of_patterns_assigned; pattern_index++)
    {
      elapsedMicros elapsed_us_inner;

      // Return if we send any command to interrupt.
      if (Serial.available())
        return;

      for (int trigger_index = 0; trigger_index < TRIGGER_OUTPUT_COUNT; trigger_index++)
        triggered[trigger_index] = true;

      // Wait for all devices to be ready to acquire (if input triggers are configured
      max_start_delay_us = 0;
      for (int trigger_index = 0; trigger_index < TRIGGER_INPUT_COUNT; trigger_index++)
      {
        if (((trigger_input_mode_list[trigger_index] > 0) && (pattern_index % trigger_input_mode_list[trigger_index] == 0))
            || ((trigger_input_mode_list[trigger_index] == TRIG_MODE_ITERATION) && (pattern_index == 0))
            || ((trigger_input_mode_list[trigger_index] == TRIG_MODE_START) && (acquisition_index == 0 && pattern_index == 0)))
        {
          waitForTriggerState(trigger_index, false);
          triggered[trigger_index] = false;
        }

        // Set the wait list to inactive state
        trigger_wait_list[trigger_index] = -0.1;
      }

      // Determine max start delay using trigger output settings
      for (int trigger_index = 0; trigger_index < TRIGGER_OUTPUT_COUNT; trigger_index++)
      {
        if (((trigger_output_mode_list[trigger_index] > 0) && (pattern_index % trigger_output_mode_list[trigger_index] == 0))
            || ((trigger_output_mode_list[trigger_index] == TRIG_MODE_ITERATION) && (pattern_index == 0))
            || ((trigger_output_mode_list[trigger_index] == TRIG_MODE_START) && (acquisition_index == 0 && pattern_index == 0)))
        {
          if (max_start_delay_us < (trigger_start_delay_list_us[trigger_index] + trigger_pulse_width_list_us[trigger_index]))
            max_start_delay_us = trigger_start_delay_list_us[trigger_index] + trigger_pulse_width_list_us[trigger_index];
        }
      }

      // Send output trigger pulses before illuminating
      elapsed_us_0 = (float) elapsed_us_inner;
      while (((float)elapsed_us_inner - elapsed_us_0) < (float)max_start_delay_us)
      {
        for (int trigger_index = 0; trigger_index < TRIGGER_OUTPUT_COUNT; trigger_index++)
        {
          if (((trigger_output_mode_list[trigger_index] > 0) && (pattern_index % trigger_output_mode_list[trigger_index] == 0))
              || ((trigger_output_mode_list[trigger_index] == TRIG_MODE_ITERATION) && (pattern_index == 0))
              || ((trigger_output_mode_list[trigger_index] == TRIG_MODE_START) && (acquisition_index == 0 && pattern_index == 0)))
          {
            if ((((float)elapsed_us_inner - elapsed_us_0) > (float)(max_start_delay_us - trigger_start_delay_list_us[trigger_index] - trigger_pulse_width_list_us[trigger_index])) && !triggered[trigger_index]) // check if we're ready to trigger
            {
              led_array_interface->sendTriggerPulse(trigger_index, trigger_pulse_width_list_us[trigger_index], false);
              triggered[trigger_index] = true;
            }
          }
        }
      }

      // Wait for all devices to be in an acquiring state (if input triggers are configured)
      elapsed_us_0 = (float) elapsed_us_inner;
      trigger_finished_waiting_count = 0;
      while (trigger_finished_waiting_count < TRIGGER_INPUT_COUNT)
      {
        trigger_finished_waiting_count = 0;
        for (int trigger_index = 0; trigger_index < TRIGGER_INPUT_COUNT; trigger_index++)
        {
          if (((trigger_input_mode_list[trigger_index] > 0) && (pattern_index % trigger_input_mode_list[trigger_index] == 0))
              || ((trigger_input_mode_list[trigger_index] == TRIG_MODE_ITERATION) && (pattern_index == 0))
              || ((trigger_input_mode_list[trigger_index] == TRIG_MODE_START) && (acquisition_index == 0 && pattern_index == 0)))
          {
            if (getTriggerState(trigger_index) && trigger_wait_list[trigger_index] < 0)
            {
              trigger_wait_list[trigger_index] = (float) elapsed_us_inner - elapsed_us_0;
              trigger_finished_waiting_count += 1;
            }
          }
          else
          {
            if (trigger_wait_list[trigger_index] < 0)
            {
              trigger_wait_list[trigger_index] = 0.0;
              trigger_finished_waiting_count += 1;
            }
          }
        }
      }

      for (int trigger_index = 0; trigger_index < TRIGGER_INPUT_COUNT; trigger_index++)
        if (trigger_wait_list[trigger_index] > 0.0)
        {
          Serial.print(F("WARNING (LedArray::runSequenceFast): Trigger "));
          Serial.print(trigger_index);
          Serial.print(F(" had excess delay of "));
          Serial.print(trigger_wait_list[trigger_index]);
          Serial.print(F("us for pattern #"));
          Serial.print(pattern_index);
          Serial.print(F(", acquisition #"));
          Serial.println(acquisition_index);
        }

      // while every trigger is not finished
      if (pattern_index == 0)
        elapsed = (float)elapsed_us_outer;

      elapsedMicros elapsed_us_display;

      // Set all LEDs to zero
      led_array_interface->setLedFast(-1, -1, 0);

      // Define pattern
      for (uint16_t led_idx = 0; led_idx < led_sequence.led_counts[pattern_index]; led_idx++)
      {
        led_number = led_sequence.led_list[pattern_index][led_idx];
        led_array_interface->setLedFast(led_number, 0, true); // assume the fast that there is a LED # implies this LED is on
        // TODO: make the above line respect color channels
      }

      // Ensure that we haven't set too short of a delay
      if ((float)elapsed_us_display > delay_us_used)
      {
        Serial.println(F("Error - delay("));
        Serial.print((float)delay_us_used);
        Serial.println(F("us) too short!"));
        return;
      }

      // Set all LEDs to zero
      led_array_interface->setLedFast(-1, -1, 0);

      // Wait for the defined mininum amount of time (delay_ms) before checking trigger input state
      while ((float)elapsed_us_display < (delay_us_used)) {} // Wait until this is true

      if (false)
      {
        Serial.print(F("Frame #"));
        Serial.print(pattern_index);
        Serial.print(F(": Elapsed time: "));
        Serial.print((float)elapsed_us_outer - elapsed);
        Serial.print(F("us (This loop: "));
        Serial.print((float)elapsed_us_inner);
        Serial.println(F("us)"));
      }
    }
  }

  // Clear LED Array
  led_array_interface->setLedFast(-1, -1, 0);

  // Wait for all devices to finish acquiring
  for (int trigger_index = 0; trigger_index < TRIGGER_INPUT_COUNT; trigger_index++)
  {
    if (trigger_input_mode_list[trigger_index] == TRIG_MODE_ITERATION || trigger_input_mode_list[trigger_index] == TRIG_MODE_START)
      waitForTriggerState(trigger_index, false);
  }

  // Print results
  if (debug)
    Serial.println(F("Finished fast Sequence"));
}

void LedArray::stepSequence(uint16_t argc, char ** argv)
{
  Serial.println(F("Stepping sequence"));

  /* Format for argv:
     0: trigger output 1 setting
     1: trigger output 2 setting
     2: trigger input 1 setting
     3: trigger input 2 setting
  */

  // Reset Trigger parameters
  trigger_output_mode_list[0] = 0;
  trigger_output_mode_list[1] = 0;
  trigger_input_mode_list[0] = 0;
  trigger_input_mode_list[1] = 0;

  for (int argc_index = 0; argc_index < argc; argc_index++)
  {
    if (argc_index >= 0 && argc_index < 2)
      trigger_output_mode_list[argc_index] = atoi(argv[argc_index]);
    else if (argc_index >= 2 && argc_index < 4)
      trigger_input_mode_list[argc_index - 2] = atoi(argv[argc_index]);
    else
      Serial.println("WARNING:  Ignoring additional argument in stepSequence");
  }

  if (debug)
  {
    Serial.println("OPTIONS:");
    Serial.print("  trigger out 0: ");
    Serial.println(trigger_output_mode_list[0]);
    Serial.print("  trigger out 1: ");
    Serial.println(trigger_output_mode_list[1]);
    Serial.print("  trigger in 0: ");
    Serial.println(trigger_input_mode_list[0]);
    Serial.print("  trigger in 1: ");
    Serial.println(trigger_input_mode_list[1]);
  }
  // Reset Trigger parameters
  trigger_output_mode_list[0] = 0;
  trigger_output_mode_list[1] = 0;


  // Incriment counter
  sequence_number_displayed++;

  // Loop sequence counter if it's at the end
  if (sequence_number_displayed >= led_sequence.number_of_patterns_assigned)
    sequence_number_displayed = 0;

  uint16_t led_number;

  // Sent output trigger pulses before illuminating
  for (int trigger_index = 0; trigger_index < TRIGGER_OUTPUT_COUNT; trigger_index++)
  {
    if (((trigger_input_mode_list[trigger_index] > 0) && (sequence_number_displayed % trigger_input_mode_list[trigger_index] == 0))
        || ((trigger_output_mode_list[trigger_index] == TRIG_MODE_ITERATION) && (sequence_number_displayed == 0))
        || ((trigger_output_mode_list[trigger_index] == TRIG_MODE_START) && (sequence_number_displayed == 0)))
    {
      sendTriggerPulse(trigger_index, false);

      if (trigger_start_delay_list_us[trigger_index] > 0)
        delayMicroseconds(trigger_start_delay_list_us[trigger_index]);
    }
  }

  // Wait for all devices to start acquiring (if input triggers are configured
  for (int trigger_index = 0; trigger_index < TRIGGER_INPUT_COUNT; trigger_index++)
  {
    if (((trigger_input_mode_list[trigger_index] > 0) && (sequence_number_displayed % trigger_input_mode_list[trigger_index] == 0))
        || ((trigger_input_mode_list[trigger_index] == TRIG_MODE_ITERATION) && (sequence_number_displayed == 0))
        || ((trigger_input_mode_list[trigger_index] == TRIG_MODE_START) && (sequence_number_displayed == 0)))
      waitForTriggerState(trigger_index, true);
  }

  elapsedMicros elapsed_us_inner;

  // Clear the array
  led_array_interface->clear();

  // Send LEDs
  for (uint16_t led_idx = 0; led_idx < led_sequence.led_counts[sequence_number_displayed]; led_idx++)
  {
    led_number = led_sequence.led_list[sequence_number_displayed][led_idx];
    for (int color_channel_index = 0; color_channel_index < led_array_interface->color_channel_count; color_channel_index++)
      led_array_interface->setLed(led_number, color_channel_index, led_sequence.values[sequence_number_displayed][led_idx]);
  }

  // Update pattern
  led_array_interface->update();

  // Wait for all devices to start acquiring (if input triggers are configured
  for (int trigger_index = 0; trigger_index < TRIGGER_INPUT_COUNT; trigger_index++)
  {
    if ((trigger_input_mode_list[trigger_index] > 0) && (sequence_number_displayed % trigger_input_mode_list[trigger_index] == 0))
      waitForTriggerState(trigger_index, true);
    else if ((trigger_input_mode_list[trigger_index] == TRIG_MODE_ITERATION) && (sequence_number_displayed == 0))
      waitForTriggerState(trigger_index, true);
    else if ((trigger_input_mode_list[trigger_index] == TRIG_MODE_START) && (sequence_number_displayed == 0))
      waitForTriggerState(trigger_index, true);
  }

  // Print user feedback
  Serial.print(F("Displayed pattern # ")); Serial.print(sequence_number_displayed); Serial.print(F(" of ")); Serial.println( led_sequence.number_of_patterns_assigned);
}

void LedArray::setDistanceZ(float new_z)
{
  if (new_z > 0)
  {
    led_array_distance_z = new_z / 100.0;
    Serial.print(F("Set LED Array distance to: "));
    Serial.print(led_array_distance_z);
    Serial.println(F("mm"));
    clearNaList();
    buildNaList(led_array_distance_z);
  }
}

void LedArray::toggleAutoClear(uint16_t argc, char ** argv)
{
  if (argc == 0)
    auto_clear_flag = !auto_clear_flag;
  else
    auto_clear_flag = (bool)atoi(argv[0]);
}

void LedArray::setDebug(uint16_t new_debug_level)
{
  // Set debug level for this file
  debug = (int) (new_debug_level % 10);

  // User feedback
  Serial.printf(F("(LedArray::setDebug): Set debug level to %d \n"), debug);

  // Set debug level for interface
  led_array_interface->setDebug((int) ((new_debug_level % 100 - new_debug_level % 10) / 10.0));
}

void LedArray::setInterface(LedArrayInterface * interface)
{
  led_array_interface = interface;
}

void LedArray::setup()
{
  // Initialize led array
  led_array_interface->deviceSetup();

  // Attach interrupts
  pinMode(TRIGGER_INPUT_PIN_0, INPUT);
  pinMode(TRIGGER_INPUT_PIN_1, INPUT);

  // Define led_value
  led_value = new uint8_t[led_array_interface->color_channel_count];
  for (int color_channel_index = 0; color_channel_index < led_array_interface->color_channel_count; color_channel_index++)
    led_value[color_channel_index] = 127; // TODO: make this respect bit depth

  // Reset sequence
  led_sequence.deallocate();

  // Initialize sequences at every bit depth so these are defined
  led_sequence.allocate(4);
  led_sequence.incriment(1);
  led_sequence.append(0, 127);
  led_sequence.incriment(1);
  led_sequence.append(1, 127);
  led_sequence.incriment(1);
  led_sequence.append(2, 127);
  led_sequence.incriment(1);
  led_sequence.append(3, 127);

  // Build list of LED NA coordinates
  buildNaList(led_array_distance_z);

  // Define default NA
  setNa(DEFAULT_NA);
}

void LedArray::demo()
{
  while (!Serial.available())
  {

    //    // Demo fill array
    //    for (int color_channel_index_outer = 0; color_channel_index_outer < led_array_interface->color_channel_count; color_channel_index_outer++)
    //    {
    //      for (int color_channel_index = 0; color_channel_index < led_array_interface->color_channel_count; color_channel_index++)
    //        led_value[color_channel_index] = 0;
    //
    //      led_value[color_channel_index_outer] = 10;
    //      led_array_interface->clear();
    //
    //      for (int color_channel_index = 0; color_channel_index < led_array_interface->color_channel_count; color_channel_index++)
    //        led_array_interface->setLed(-1, color_channel_index_outer, led_value[color_channel_index_outer]);
    //
    //      led_array_interface->update();
    //      delay(250);
    //    }

    // Demo Brightfield patterns
    for (int color_channel_index_outer = 0; color_channel_index_outer < led_array_interface->color_channel_count; color_channel_index_outer++)
    {
      for (int color_channel_index = 0; color_channel_index < led_array_interface->color_channel_count; color_channel_index++)
        led_value[color_channel_index] = 0;

      led_value[color_channel_index_outer] = 64;
      led_array_interface->clear();
      drawCircle(0, objective_na);
      led_array_interface->update();
      delay(250);
    }

    if (Serial.available())
      return;

    // Demo Annulus patterns
    for (int color_channel_index_outer = 0; color_channel_index_outer < led_array_interface->color_channel_count; color_channel_index_outer++)
    {
      for (int color_channel_index = 0; color_channel_index < led_array_interface->color_channel_count; color_channel_index++)
        led_value[color_channel_index] = 0;

      led_value[color_channel_index_outer] = 64;
      led_array_interface->clear();
      drawCircle(objective_na, objective_na + 0.2);
      led_array_interface->update();
      delay(250);
    }

    if (Serial.available())
      return;

    // Demo DPC Patterns
    for (int color_channel_index_outer = 0; color_channel_index_outer < led_array_interface->color_channel_count; color_channel_index_outer++)
    {
      for (int dpc_index = 0; dpc_index < 4; dpc_index++)
      {
        for (int color_channel_index = 0; color_channel_index < led_array_interface->color_channel_count; color_channel_index++)
          led_value[color_channel_index] = 0;

        led_value[color_channel_index_outer] = 127;

        led_array_interface->clear();
        drawHalfCircle(dpc_index, 0, objective_na);
        led_array_interface->update();

        delay(250);
      }
    }

    if (Serial.available())
      return;

    for ( int16_t led_index = 0; led_index <  led_array_interface->led_count; led_index++)
    {
      led_array_interface->setLed(-1, -1, (uint8_t)0);
      led_array_interface->setLed(led_index, -1, (uint8_t)127);
      led_array_interface->update();
      delay(1);
      if (Serial.available())
        return;
    }

    for ( int16_t led_index = led_array_interface->led_count - 1; led_index >= 0; led_index--)
    {
      led_array_interface->setLed(-1, -1, (uint8_t)0);
      led_array_interface->setLed(led_index, -1, (uint8_t)127);
      led_array_interface->update();
      delay(1);
      if (Serial.available())
        return;
    }

    delay(100);
  }
}

void LedArray::notImplemented(const char * command_name)
{
  Serial.print(F("Command "));
  Serial.print(command_name);
  Serial.println(F(" is not implemented for this device."));
}

int LedArray::getColorChannelCount()
{
  return led_array_interface->color_channel_count;
}
