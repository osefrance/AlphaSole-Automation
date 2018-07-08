//function that is called every 5 minutes and save the data from the sensors
void saveDatalogger()
{
  char file[20], saved_data[100];
  
  sprintf(file,"day%d.csv",nb_day);
  //sprintf(saved_data,"%d;%d;%d;%d;%d;%d",nbCapture,power,temp1,temp2,pressure);
  //Here it is pin 53 for Arduino Mega but for arduino Uno you need to change
  if (!SD.begin(53)) {
    flagBug = 1;
    Serial.println(F("initialization failed! ; BUG"));
  }
  Serial.println(F("initialization done."));

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open(file, FILE_WRITE);
  // if the file opened okay, write to it:
  if (myFile) {
    Serial.println(F("Writing..."));
    //myFile.println(saved_data);
    myFile.print(nbCapture);
    myFile.print(";");
    myFile.print(power);
    myFile.print(";");
    myFile.print(temp1);
    myFile.print(";");
    myFile.print(temp2);
    myFile.print(";");
    myFile.print(flowRate);
    myFile.print(";");
    myFile.println(pressure);
    // close the file:
    myFile.close();
    Serial.println(F("done."));
  } else {
    // if the file didn't open, print an error:
    flagBug = 1;
    Serial.println(F("error opening texte file ; BUG"));
  }    
}


//function that is called each day to create and initialize a new file stored in the SD memory card
void initDatalogger()
{
  char file[20], saved_data[100];
    
  sprintf(file,"day%d.csv",nb_day);
  //Here it is pin 53 for Arduino Mega but for arduino Uno you need to change
  if (!SD.begin(53)) {
    flagBug = 1;
    Serial.println(F("initialization failed! ; BUG"));
  }
  Serial.println(F("initialization done."));

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  SD.remove(file);
  myFile = SD.open(file, FILE_WRITE);
  // if the file opened okay, write to it:
  if (myFile) {
    Serial.println(F("Writing..."));
    myFile.println("save_nb;Puissance(W);Temperature Debut (°C);Temperature Fin (°C);Debit(L/min);Pression(bar)");
    // close the file:
    myFile.close();
    Serial.println(F("done."));
  } else {
    // if the file didn't open, print an error:
    flagBug = 1;
    Serial.println(F("error opening texte file ; BUG"));
  }  
}

//function called at the end of the day. save the motor positions on the SD card in sun.txt file. 
void refreshHeliostat()
{
  char file[10], saved_data[100];
  int i;
  sprintf(file,"sun.txt");

  //Here it is pin 53 for Arduino Mega but for arduino Uno you need to change
  if (!SD.begin(53)) {
    flagBug = 1;
    Serial.println(F("initialization failed! ; BUG"));
  }
  Serial.println(F("initialization done."));

  //delete the file for rewrite all the data.
  SD.remove(file);
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open(file, FILE_WRITE);
  // if the file opened okay, write to it:
  if (myFile) {
    Serial.println(F("Writing..."));
    for( i = 0; i < TAKES_PER_DAY; i++)
    {
      //sprintf(saved_data,"%d %d",nbCapture,forecast[i]);
      myFile.print(i);
      myFile.print(" ");
      myFile.println(forecast[i]);
    }
    
    // close the file:
    myFile.close();
    Serial.println(F("done."));
  } else {
    flagBug = 1;
    // if the file didn't open, print an error:
    Serial.println(F("error opening texte file ; BUG"));
  }
}

//function called at the beginning of the day. Load the motor positions from the SD card on a buffer.
void initHeliostat()
{
  char file[10];
  int index,val,space_index;
  char a;
  String b = "", S_index, S_val;
  sprintf(file,"sun.txt");

  //Re-open the file for reading:
  myFile = SD.open(file);
  if (myFile) {
    Serial.println(file);

    //Read from the file until there's nothing else in it:
    while (myFile.available()) {
      b = "";
      a=' ';
      while(a !='\r')
       {
        //Serial.println(a); if the program is stuck here we need to add a new line at the end on the document sun.txt
        a = myFile.read();
        b += a;
       }
      space_index = b.indexOf(" ");
      S_index = b.substring(0,space_index);
      S_val = b.substring(space_index+1);
      index = S_index.toInt();
      val = S_val.toInt();
      forecast[index] = val;
      if(index==TAKES_PER_DAY-1){
        break;  
      }
      
    }
    // close the file:
    myFile.close();
    nbCapture = 0;
  } else {
    // if the file didn't open, print an error:
    flagBug = 1;
    Serial.println(F("error opening texte file ; BUG"));
  }
}
