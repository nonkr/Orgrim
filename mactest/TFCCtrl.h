#ifndef __TFC_CTRL_H__
#define __TFC_CTRL_H__

__BEGIN_DECLS

/*Function description: Open TOF device 
 *Input parameter:
 *    NULL
 *Output parameter:
 *    NULL
 *Return value:
 *    <fd number> File descriptor of tof device
 *    <-1>open faiure
 */
int openTfcDev(void);

/*Function description: Set tof working under MAC_TEST mode
 *Input parameter:
 *    <fd> File descriptor of tof device 
 *Output parameter:
 *    NULL
 *Return value:
 *    <0> setting success
 *    <-1>setting faiure
 */
int setMacTestMode(const int fd);

/*Function description: close MAC_TEST mode
 *Input parameter:
 *    <fd> File descriptor of tof device 
 *Output parameter:
 *    NULL
 *Return value:
 *    <0> setting success
 *    <-1>setting faiure
 */
int clrMacTestMode(const int fd);

/*Function description: Check the current ampilitude data to pattern data.
 *Input parameter: 
 *   <data> A pointer which point to current ampilitude data buffer
 *          Attention*: ampilitude data have no flag bits.
 *   <datasize> ampilitude data size
 *Output parameter:
 *   NULL
 *Return Value:
 *   <-1> Invalid input parameter
 *   <0> Not Match
 *   <1> Match
 */
int checkPattern(const unsigned short *data, const int datasize);

/*Function description: Close TOF device 
 *Input parameter:
 *    <fd> File descriptor of tof device 
 *Output parameter:
 *    NULL
 *Return value:
 *    NULL
 */
void closeTfcDev(int fd);

__END_DECLS

#endif
