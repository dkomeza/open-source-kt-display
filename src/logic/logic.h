#ifndef TORQUESENSOR_H
#define TORQUESENSOR_H

class Logic {
    private:
        int calculateUpCRC();
        bool shiftArray(int counter);
    public:
        void processPacket();
}

#endif