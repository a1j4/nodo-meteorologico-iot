#include <Arduino.h>
#include "DataBuffer.h"
//implementa una memoria circular
//Guarda hasta 10 datos y cuando se llena, empieza a sobrescribir los más antiguos
DataBuffer::DataBuffer(){
    index =0;// primera posicion donde se guarda 
    count =0;//no hay dato
}
// guarda los datos en la pocicion actual
void DataBuffer::add(const SensorData& data){
    buffer[index]=data;
    index = (index + 1) %  BUFFER_SIZE;
    if(count < BUFFER_SIZE){//cuaenta cuantos datos hay
        count++;
    }
}//permite obtener un dato del y verificar si existe o no el dato
SensorData DataBuffer::get(int position){
    if (position < 0 ||position >= count)
    {
        SensorData empty;
        empty.valid=false;
        return empty;
    }
    
    return buffer[position];
}
//debuelbe cuantos datos hay guardados 
int DataBuffer::size(){
    return count;
}