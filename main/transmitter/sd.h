//sd variables
String nameFileS = "";
char nameFile[50];
int regist = 1;
int numFile = 1; //used only if the datetime has not synchronized

File dataFile;

// sd functions
void create_file_sd(String datetime) {
  //rename file
  if (datetime != "0-0-2000_0-0-0") { 
    nameFileS =  "/data-volta_" + datetime + ".txt" ;
  } else {
    nameFileS =  "/data-volta_" + String(numFile) + ".txt" ;
    numFile ++;
  }
  nameFileS.toCharArray(nameFile, sizeof(nameFile));
  //creating file
  dataFile = SD.open(nameFile, FILE_WRITE); 
  //trying to create the file
  if (!dataFile) {
    Serial.printf("Error creando to file: %s\n", nameFile);
  } else {
    Serial.printf("Creando to file: %s\n", nameFile);
  }
}

void save_data_sd(String datetime, String dataBuffer) {
    if (regist <= 50) {
    dataFile.println(dataBuffer);
    dataFile.flush();
    regist ++;
  } else {
    dataFile.close();
    create_file_sd(datetime);
    dataFile.println(dataBuffer);
    dataFile.flush();
    regist = 1;
  }
}
