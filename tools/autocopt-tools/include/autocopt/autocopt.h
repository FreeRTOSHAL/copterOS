#ifndef AUTOCOPT_H_U_
#define AUTOCOPT_H_U_
#include <stdint.h>
#include <sys/types.h>
#include <linux/autocopt.h>

#ifndef BIT
/**
 * Set a Bit in a Bitmask
 * \param x Bit Number
 */
# define BIT(x) (1 << x)
#endif
#ifndef IS_BIT_SET
/**
 * Check if Bit is Set
 * \param x Value
 * \param p Bit Number
 */
# define IS_BIT_SET(x, p) (((x) >> p) & 0x1)
#endif

#define AUTOCOPT_MODE_SPECTRUM 0
#define AUTOCOPT_MODE_LINUX 1

/**
 * Autocopt Handle
 */
struct autocopt;

/**
 * Default Device
 */
#define AUTOCOPT_DEFAULT_DEV "/dev/copt0"
/**
 * Init Function
 * \param dev String to dev for Example AUTOCOPT_DEFAULT_DEV
 * \return Autocopt Handle or NULL on Error
 */
struct autocopt *autocopt_init(char *dev);
/**
 * Free Dev and Device
 * \param copt Autocopt Handle
 * \return -1 on Error 0 on ok 
 */
int32_t autocopt_deinit(struct autocopt *copt);

/**
 * Low - Level Send Function
 * \param copt Autocopt Handle
 * \param msg Message
 * \param len Size to write
 * \return see write(2)
 */
int32_t autocopt_send(struct autocopt *copt, struct autocopt_msg *msg, size_t len);
/**
 * Low - level Recv Function
 * \param copt Autocopt Handle
 * \param msg recv Message
 * \param len size
 * \return see read(2)
 */
int32_t autocopt_recv(struct autocopt *copt, struct autocopt_msg *msg, size_t len);

/**
 * Ping Copter
 * Use SIG_ALAM for Timeout
 * \todo Do not use SIG_ALAM
 * \parma copt Autocopt Handle
 * \return -1 on error 0 ok 
 */ 
int32_t autocopt_ping(struct autocopt *copt);

/**
 * Select Flight Mode
 * \param copt Autocopt Handle 
 * \param mode Flight Mode: AUTOCOPT_MODE_SPECTRUM or AUTOCOPT_MODE_LINUX
 * \return -1 on error 0 on ok
 */
int32_t autocopt_select(struct autocopt *copt, uint32_t mode);

/**
 * Autocopt Control Message
 */
struct autocopt_control {
	/**
	 * -1 left +1 right
	 */ 
	float roll;
	/**
	 * -1 back +1 forward
	 */
	float pitch;
	/**
	 * -1 left +1 right
	 */
	float yaw;
	/**
	 * 0 = no Thrust
	 * 2^16 = full Thrust
	 * \waring Value below 60000 the copter may instable 
	 */
	uint16_t thrust;
} __attribute__((packed));

/**
 * Control the Copter Set roll, pitch, yaw and thrust of the copter
 * \param copt copt Handle
 * \param ctl roll, pitch, yaw and thrust
 * \todo Timeout over SIG_ALAM
 * \return -1 on Error and 0 on ok 
 */ 
int32_t autocopt_control(struct autocopt *copt, struct autocopt_control *ctl);

/**
 * Set Roll in Angle Mode
 */ 
#define AUTOCOPT_PID_ANGLE_MODE_R BIT(0)
/**
 * Set Pitch in Angle Mode
 */ 
#define AUTOCOPT_PID_ANGLE_MODE_P BIT(1)
/**
 * Set Yaw in Angle Mode
 */ 
#define AUTOCOPT_PID_ANGLE_MODE_Y BIT(2)

/**
 * Send Setting for Rate PID Contoller
 */
#define AUTOCOPT_PID_TYPE_RATE 0
/**
 * Send Setting for Angle PID Contoller
 */
#define AUTOCOPT_PID_TYPE_ANGLE 1

/**
 * PID Message Header
 */
struct autocopt_pidValuesHeader {
	/**
	 * Type Values: PID_TYPE_RATE or PID_TYPE_ANGLE
	 */
        uint8_t type;
	/**
	 * Enable Angle Mode (Bitmask)
	 * Values: PID_ANGLE_MODE_R PID_ANGLE_MODE_P PID_ANGLE_MODE_Y
	 * Example:
	 * \code{.c}
	 * struct pidValues val = {
	 *   .header.mode = PID_ANGLE_MODE_R | PID_ANGLE_MODE_P,
	 * };
	 * \endcode
	 */
        uint8_t mode;
} __attribute__((packed));

/**
 * PID Message
 * 
 * \waring Overwriting PID Setting of Copter. Can make Copter instable! 
 */
struct autocopt_pidValues{
	/**
	 * Header 
	 * \see struct autocopt_pidValuesHeader
	 */
        struct autocopt_pidValuesHeader header;
	/**
	 * P Value of Roll
	 */
        uint16_t KpR;
	/**
	 * I Value of Roll
	 */
        uint16_t KiR;
	/**
	 * D Value of Roll
	 */
        uint16_t KdR;
	/**
	 * P Value of Pitch
	 */
        uint16_t KpP;
	/**
	 * I Value of Pitch
	 */
        uint16_t KiP;
	/**
	 * D Value of Pitch
	 */
        uint16_t KdP;
	/**
	 * P Value of Yaw
	 */
        uint16_t KpY;
	/**
	 * I Value of Yaw
	 */
        uint16_t KiY;
	/**
	 * D Value of Yaw
	 */
        uint16_t KdY;
} __attribute__((packed));

/**
 * Set PID Values of Copter 
 * \param copt Autocopt Handle
 * \param values PID Values
 * \waring Overwriting PID Setting of Copter. Can make Copter instable! 
 * \return 0 if OK -1 on can't send
 */
int32_t autocopt_pid(struct autocopt *copt, struct autocopt_pidValues *values);
#endif
