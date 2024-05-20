#ifndef PAGE_H
#define PAGE_H

const char MAIN_PAGE[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Content Box Example</title>
    <style>
        body {
            background-color: #f0f0f0; 
            font-family: Arial, sans-serif; 
        }

        .cover {
            max-width: 500px;
            height: 500px;
            margin: auto;
            border-radius: 20px;
            background-color: rgb(194, 191, 191);
            box-shadow: 0 0 10px rgba(88, 87, 87, 0.3); 
            overflow: auto;
        }

        .container {
            max-width: 80%; 
            margin: 20px auto; 
            text-align: center;
            overflow: auto;
        }

        .content-box {
            height: 200px;
            max-width: 600px; 
            margin: 20px auto; 
            border-radius: 10px; 
            padding: 20px; 
            background-color: #ffffff; 
            box-shadow: 0 0 10px rgba(88, 87, 87, 0.3); 
            overflow: auto; 
        }

        .content-box p {
            line-height: 1.6; 
        }
        
        .btn {
            
            background-color:#adff2f;
            border: none;
            color: white;
            padding: 10px 20px;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 16px;
            margin: 4px 2px;
            cursor: pointer;
        }

        .indicator {
            background-color: rgb(121, 121, 121);
            height: 200px;
            max-width: 600px; 
            margin: 20px auto; 
            border-radius: 10px; 
            padding: 20px; 
            background-color: #ffffff; 
            box-shadow: 0 0 10px rgba(88, 87, 87, 0.3); 
            overflow: auto;
        }
    
    </style>
</head>

<body onload="sendData()">
    <div class="cover">
        <div class="container">

            <h2>Entry/Exit Log</h2>
            <div class="content-box" id="logBox">
                <p></p>
            </div>

            <div class="indicator" id="PIR"></div>
            <div class="indicator" id="HcSR04">
                <p></p>
            </div>
            
            <button id="reset" onclick="resetEvent()">reset</button>
        </div>
    </div>
</body>

<script type="text/javascript">
    //document object
    let logBox = document.getElementById("logBox");
    let LEDbutton = document.getElementById("LEDbutton");
    let PIR = document.getElementById("PIR");
    let HcSR04 = document.getElementById("HcSR04");
    let enter = 0;
    let exit = 0;

    

    function sendData(){
        var client = new XMLHttpRequest();
        client.onreadystatechange = () => {
            if(client.readyState == 4 && client.status == 200){
                let localDate = new Date();
                let response = client.responseXML;

                //PIR_detect
                let PIR_detect = response.getElementsByTagName("PIR_detect")[0].textContent;
                if(PIR_detect === "1"){
                    PIR.style.backgroundColor = "#adff2f";

                } else if(PIR_detect === "0"){
                    PIR.style.backgroundColor = "rgb(121, 121, 121)";
                    
                } else {console.log("error at sendData.PIR_detect");console.log(PIR_detect);}

                //HcSR04_ready
                let HcSR04_ready = response.getElementsByTagName("HcSR04_ready")[0].textContent;
                let HcSR04_distance = response.getElementsByTagName("HcSR04_distance")[0].textContent;
                if(HcSR04_ready === "1"){
                    HcSR04.style.backgroundColor = "#adff2f";

                } else if(HcSR04_ready === "0"){
                    HcSR04.style.backgroundColor = "rgb(121, 121, 121)";
                    
                } else {console.log("error at sendData.HcSR04_ready");console.log(HcSR04_ready);}
                HcSR04.innerHTML = "<p>"+HcSR04_distance+"</p>" + HcSR04.innerHTML;

                
                //HcSR04_distance
                

                //direction
                let direction = response.getElementsByTagName("direction")[0].textContent;
                if(direction === "Enter"){
                    let text = "<p>"+ "Enter at "+ localDate.getHours() + ":" + localDate.getMinutes() + ":" + localDate.getSeconds() + "</p>";
                    logBox.innerHTML = text + logBox.innerHTML; 
                    //enter++;
                } else if(direction === "Exit"){
                    let text = "<p>"+ "Exit  at "+ localDate.getHours() + ":" + localDate.getMinutes() + ":" + localDate.getSeconds() + "</p>";
                    logBox.innerHTML = text + logBox.innerHTML;
                    //exit++; 
                } else if(direction === "0"){}
                else {console.log("error at sendData");console.log(response);}
                /*  
    if(PIR_detect){
      text += "<PIR_detect>1</PIR_detect>";
    } else {
      text += "<PIR_detect>0</PIR_detect>";
    }

    if(HcSR04_ready){
      text += "<HcSR04_ready>1</HcSR04_ready>";
    } else {
      text += "<HcSR04_ready>0</HcSR04_ready>";
    }
    
    text += "<HcSR04_distance>";
    text += HcSR04_distance ;
    text += "</HcSR04_distance>";

    if(from_HcSr04){
      text += "<direction>Enter</direction>";
      hw508_start(hw508_f_enter);
      from_HcSr04 = 0;
      
    } else if(from_PIR){
      text += "<direction>Exit</direction>";
      hw508_start(hw508_f_exit);
      from_PIR = 0;
      
    } else {
      text += "<direction>0</direction>";
      
    }
    text+= "</DATA>";


                 */
            }       
        }
        client.open("PUT", "REQ_sendData");
        client.send();
        
        setTimeout(sendData, 50);
    }

    function resetEvent(){
        logBox.innerHTML = "";
    }
    

    
 

</script>
</html>
)=====";
#endif
