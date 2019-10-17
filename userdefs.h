#define HOURS_STAY_ACTIVE 5

#define LEDON           LOW
#define LEDOFF          HIGH
#define LED_PIN_OUT     12     //D8
#define LED_ONBOARD     2
#define PIR_PIN_IN      5
#define PUMP_PIN_OUT    14
#define PULSE_PERIOD    700     // The period (on/off) of a burst in ms
#define BARRAGE_LENGTH  3000    // How long a barrage lasts
#define BURST_LENGTH    300     // How long to run pump during a burst
#define QUIET_PERIOD    10000   // Startup quiet period, ms
#define PUMPOFF         LOW
#define PUMPON          HIGH
#define PAUSE           10000   // Minimum time between barages
#define DEBOUNCE        4000    // Needs to be longer than the ghost triggers for cheap PIR
#define ALIVE_PULSE_PERIOD  10000
#define ALIVE_PULSE_LENGTH   200

#define REFRESH         30      // page refresh seconds
#define PAGE_LENGTH     700
#define BASEPAGE  "<meta http-equiv=\"refresh\" content=\"%d; URL=/\" >\
 <h1>Catling-Gun Prototype</h1>\
 <p>Status: %s</p>\
 <p>Detection count: %d</p>\
 <p>Shots fired: %d</p>\
 <p><a href=\"%s\"><button>%s</button></a>&nbsp;\
 <a href=\"reset-count\"><button>Reset Count</button></a> </p>\
 <p>Active time left: %d:%dm</p>\
 <p><a href=\"reset-clock\"><button>Reset Clock</button></a> </p>\
 <p>Average time: %dms</p>\
 <p> Update firmware.bin: %s</p>\
 %s"


#define SECONDS_IN_MS     1000
#define SECONDS_IN_MINUTE 60
#define MINUTES_IN_HOURS  60
#define ACTIVE_MSEC       HOURS_STAY_ACTIVE * MINUTES_IN_HOURS * SECONDS_IN_MINUTE * SECONDS_IN_MS
