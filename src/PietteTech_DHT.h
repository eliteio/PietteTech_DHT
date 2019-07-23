/*
 * FILE:        PietteTech_DHT.h
 * VERSION:     0.4
 * PURPOSE:     Particle Interrupt driven lib for DHT sensors
 * LICENSE:     GPL v3 (http://www.gnu.org/licenses/gpl.html)
 *
 * S Piette (Piette Technologies) scott.piette@gmail.com
 *      January 2014        Original Spark Port
 *      October 2014        Added support for DHT21/22 sensors
 *                          Improved timing, moved FP math out of ISR
 *      September 2016      Updated for Particle and removed dependency
 *                          on callback_wrapper.  Use of callback_wrapper
 *                          is still for backward compatibility but not used
 * ScruffR
 *      February 2017       Migrated for Libraries 2.0
 *                          Fixed blocking acquireAndWait()
 *                          and previously ignored timeout setting
 *      January  2019       Updated timing for Particle Mesh devices
 *                          issue: https://github.com/particle-iot/device-os/issues/1654
 *
 * Based on adaptation by niesteszeck (github/niesteszeck)
 * Based on original DHT11 library (http://playgroudn.adruino.cc/Main/DHT11Lib)
 *
 *
 * With this library connect the DHT sensor to the following pins
 * Spark Core: D0, D1, D2, D3, D4, A0, A1, A3, A5, A6, A7
 * Particle  : any Pin but D0 & A5
 * See docs for more background
 *   https://docs.particle.io/reference/firmware/photon/#attachinterrupt-
 *
 */

#ifndef __PIETTETECH_DHT_H__
#define __PIETTETECH_DHT_H__

 // There appears to be a overrun in memory on this class.  For now please leave DHT_DEBUG_TIMING enabled
#define DHT_DEBUG_TIMING        // Enable this for edge->edge timing collection

#include <Particle.h>
#include <math.h>

const char DHTLIB_VERSION[]              = "0.0.10";

// device types
const int  DHT11                         = 11;
const int  DHT21                         = 21;
const int  AM2301                        = 21;
const int  DHT22                         = 22;
const int  AM2302                        = 22;
                                            
// state codes                              
const int  DHTLIB_OK                     =  0;
const int  DHTLIB_ACQUIRING              =  1;
const int  DHTLIB_ACQUIRED               =  2;
const int  DHTLIB_RESPONSE_OK            =  3;
                                            
// error codes                              
const int  DHTLIB_ERROR_CHECKSUM         = -1;
const int  DHTLIB_ERROR_ISR_TIMEOUT      = -2;
const int  DHTLIB_ERROR_RESPONSE_TIMEOUT = -3;
const int  DHTLIB_ERROR_DATA_TIMEOUT     = -4;
const int  DHTLIB_ERROR_ACQUIRING        = -5;
const int  DHTLIB_ERROR_DELTA            = -6;
const int  DHTLIB_ERROR_NOTSTARTED       = -7;

#define DHT_CHECK_STATE                    \
        detachISRIfRequested();            \
        if(_state == STOPPED)              \
            return _status;			           \
        else if(_state != ACQUIRED)		     \
            return DHTLIB_ERROR_ACQUIRING; \
        if(_convert) convert();

class PietteTech_DHT
{
public:
  // either this combination of constructor and begin() call
  PietteTech_DHT(uint8_t sigPin, uint8_t dht_type, void(*callback_wrapper)() = NULL);
  void begin();
  // or this
  PietteTech_DHT();
  void begin(uint8_t sigPin, uint8_t dht_type, void(*callback_wrapper)() = NULL);

  /*
   * NOTE:  isrCallback is only here for backwards compatibility with v0.3 and earlier
   *        it is no longer used or needed
   */
  void isrCallback();
  int acquire();
  int acquireAndWait(uint32_t timeout = 0);
  float getCelsius();
  float getFahrenheit();
  float getKelvin();
  double getDewPoint();
  double getDewPointSlow();
  float getHumidity();
  bool acquiring();
  int getStatus();
  float readTemperature();
  float readHumidity();
#if defined(DHT_DEBUG_TIMING)
  volatile uint8_t _edges[41];
#endif

private:
  void _isrCallback();
  void convert();
  void detachISRIfRequested();

  enum states { RESPONSE = 0, DATA = 1, ACQUIRED = 2, STOPPED = 3, ACQUIRING = 4 };
  volatile states _state;
  volatile int _status;
  volatile uint8_t _bits[5];
  volatile uint8_t _cnt;
  volatile uint8_t _idx;
  volatile unsigned long _us;
  volatile bool _convert;
  volatile bool _detachISR;
#if defined(DHT_DEBUG_TIMING)
  volatile uint8_t *_e;
#endif
  int _sigPin;
  int _type;
  unsigned long _lastreadtime;
  bool _firstreading;
  float _hum;
  float _temp;
};
#endif
