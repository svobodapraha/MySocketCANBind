#ifndef PROCESSCANMESSAGE_H
#define PROCESSCANMESSAGE_H

    typedef struct XMC_LMOCan
    {
      uint32_t    can_identifier;
      uint8_t     can_data_length;
      uint8_t     can_data[8];
    } XMC_LMOCan_t;



#endif // PROCESSCANMESSAGE_H
