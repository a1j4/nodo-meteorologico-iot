#include <Arduino.h>
#include "DataBuffer.h"


DataBuffer::DataBuffer(){
    index =0;
    count =0;
}
void DataBuffer::add(const SensorData& data){
    buffer[index]=data;
    index = (index + 1) %  BUFFER_SIZE;
    if(count < BUFFER_SIZE){
        count++;
    }
}
SensorData DataBuffer::get(int position){
    if (position < 0 ||position >= count)
    {
        SensorData empty;
        empty.valid=false;
        return empty;
    }
    
    return buffer[position];
}
int DataBuffer::size(){
    return count;
}