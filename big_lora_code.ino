#include <SPI.h>
#include <LoRa.h>
#include <dht.h>
#include <WiFi.h>
#include <WebServer.h>
#define dht_apin A0
dht DHT;

float temperature,humidity;

/* Put your SSID & Password */
const char* ssid = "Try_01";  // Enter SSID here
const char* password = "12345678";  //Enter Password here

/* Put IP Address details */
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

// Declare an object of WebServer library
WebServer server(80);

void loraActivateFn(){
	Serial.begin(9600);
	while (!Serial);
	Serial.println("LoRa Sender");
	LoRa.setSPIFrequency(4E6);
	LoRa.setPins(10, 9, 2);
	if (!LoRa.begin(433E6)) {
		Serial.println("Starting LoRa failed!");
		while (!LoRa.begin(433E6))//;
		{     Serial.println("Trying...");    }
	}
	LoRa.setTxPower(20);
}

void lupfn(){
	
	int packetSize = LoRa.parsePacket();
	char arr[60];
	
	if (packetSize){
		int i=0;
		while (LoRa.available()) { //Loop to store all incoming data in array
			arr[i]=(char)LoRa.read();
			i++;
		}
		
		String str = String(arr); //Converted to string
		
		//if(str.charAt(0) == 'a'){			Serial.println("u killin it!");		}
		
		//if(str.startsWith('a') && str.endsWith('a')){ //Ideal packet checking from node A
		if(str.charAt(0)=='a' && str.charAt(11)=='a'){ //Ideal packet checking from node A
			Serial.println("Satisfying at the moment!");
			
			char arrA[32]; //nodeA no bdho content aavse ie 3 records of Humidity and temperature
			int temp = 1; //temporary variable for linear scan 
			int h[3],t[3]; //final humidity & tempr values
			
			//Hve aa while loop 3 vaar feravvani che
			for(int lv=0;lv<3;lv++){
				while(str.charAt(temp)!='a'){ //Separation packet wise  //Aju khotu che
					//temp2 = 0;
					String tempStrH;
					char tempArr[5];
					
					//Mota array ne nana array ma laavse aa loop
					for(int stringFiller = 0; stringFiller<5;stringFiller++){
						tempArr[s`tringFiller]=str.charAt(temp++);
					}
					
					tempStrH = tempArr; //Nana array ne string ma
					h[lv]=tempStrH.toInt(); //String to int
					
					/*Humidity thai gyu 1record */
					
					//temp2 = 0;
					String tempStrT;
					char tempArr1[5];
					
					//Mota array ne nana array ma laavse aa loop
					for(int stringFiller = 0; stringFiller<5;stringFiller++){
						tempArr1[stringFiller]=str.charAt(temp++);
					}
					tempStrT = tempArr; 
					t[lv]=tempStrT.toInt();
			
					/*Temperature thai gyu ek record*/
				}
			}
			temperature = (t[1]+t[2]+t[3])/3;
			humidity = (h[1]+h[2]+h[3])/3;
		}
	}
}

void wififn(){
	WiFi.begin(ssid, password);
	//check wi-fi is connected to wi-fi network
	while (WiFi.status() != WL_CONNECTED) {
		delay(1000);
		Serial.print(".");
	}
	Serial.println("");
	Serial.println("WiFi connected..!");
	Serial.print("Got IP: ");  Serial.println(WiFi.localIP());

	server.on("/", handle_OnConnect);
	//server.on("/led1on", handle_led1on);
	//server.on("/led1off", handle_led1off);
	server.onNotFound(handle_NotFound);

	server.begin();
	Serial.println("HTTP server started");
}

void handle_OnConnect(){
	//LED1status = LOW;
	Serial.println("GPIO4 Status: OFF");
	server.send(200, "text/html", SendHTML(humidity, temperature)); 
}

void handle_NotFound(){
	server.send(404, "text/plain", "Not found");
}

String SendHTML(float humidity, float temperature){
	String ptr = "<!DOCTYPE html> <html>\n";
	ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
	ptr +="<title>Forest Status</title>\n";
	/*ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
	ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
	ptr +=".button {display: block;width: 80px;background-color: #3498db;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
	ptr +=".button-on {background-color: #3498db;}\n";
	ptr +=".button-on:active {background-color: #2980b9;}\n";
	ptr +=".button-off {background-color: #34495e;}\n";
	ptr +=".button-off:active {background-color: #2c3e50;}\n";
	ptr +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
	ptr +="</style>\n";*/
	ptr +="</head>\n";
	ptr +="<body>\n";
	ptr +="<h1>Sensor reradings</h1>\n";
	ptr +="<h3>Using Station Mode(STA) Mode</h3>\n";

	ptr += "<p>Humidity:";
	if(humidity){
		//ptr +="<p>LED1 Status: ON</p><a class=\"button button-off\" href=\"/led1off\">OFF</a>\n";
		ptr += String(humidity);
		ptr += " </p>";
	}
	else{
		//ptr +="<p>LED1 Status: OFF</p><a class=\"button button-on\" href=\"/led1on\">ON</a>\n";
		ptr+="Something's wrong!</p>";
	}

	ptr += "<p>Temperature:";
	if(temperature){
		//ptr +="<p>LED1 Status: ON</p><a class=\"button button-off\" href=\"/led1off\">OFF</a>\n";
		
		ptr += String(temperature);
		ptr += " </p>";
	}
	else{
		//ptr +="<p>LED1 Status: OFF</p><a class=\"button button-on\" href=\"/led1on\">ON</a>\n";
		ptr+="Something's wrong!</p>";
	}

	ptr +="</body>\n";
	ptr +="</html>\n";
	return ptr;
}

void setup(){ 

	loraActivateFn(); //Self defining fn
	
	lupfn(); //aama chene recieved data pr processing krsu
	
	wififn(); //wifi chalu krvamate nu fn
}

void loop(){ 
	//Loop ma kai nai

}
