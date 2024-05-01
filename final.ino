#include <WiFi.h>
#include <WebServer.h>

const char *ssid = "ESP32-Calculator";
const char *password = "your_password";

WebServer server(80);

String input = ""; // Variable to store user input

void handleRoot() {
  String page = "<html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
  page += "<style>body{font-family: bradley hand, cursive ;text-align:center;background-color: #f0f0f0;}.calculator{display: inline-block;margin: auto;text-align: center;border: 1px solid #000;border-radius: 10px;padding: 20px;background-color: #fff;box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0.2);}.screen{width: calc(100% - 30px);height: 80px;background-color: #3399FF;border: 1px solid #000;margin-bottom: 10px;text-align: right;padding: 10px;font-size: 35px;margin-bottom: 10px;border-radius: 10px;}.keypad{display: grid;grid-template-columns: repeat(4, 1fr);grid-gap: 10px;text-align: center;}.key{padding: 20px;font-size: 30px;border: 1px solid #ccc;border-radius: 10px;cursor: pointer;background-color: #FCF3CF;color: #333;}.key:hover{background-color: #ddd;}.delete{grid-column: span 2;background-color: #ff6961;color: #fff;border: none;}.clear{background-color: #4682B4;color: #fff;border: none;}</style>";
  page += "</head><body><div class=\"calculator\">";
  page += "<h1 style=\"color: #FFD700;\">Welcome to ESP32 Calculator! <br> By Aditya Waghaskar!! </h1>";
  page += "<div class=\"screen\" id=\"screen\">" + input + "</div>";
  page += "<div class=\"keypad\">";
  page += "<div class=\"key\" onclick=\"appendInput('7')\">7</div>";
  page += "<div class=\"key\" onclick=\"appendInput('8')\">8</div>";
  page += "<div class=\"key\" onclick=\"appendInput('9')\">9</div>";
  page += "<div class=\"key\" onclick=\"appendInput('/')\">/</div>";
  page += "<div class=\"key\" onclick=\"appendInput('4')\">4</div>";
  page += "<div class=\"key\" onclick=\"appendInput('5')\">5</div>";
  page += "<div class=\"key\" onclick=\"appendInput('6')\">6</div>";
  page += "<div class=\"key\" onclick=\"appendInput('*')\">*</div>";
  page += "<div class=\"key\" onclick=\"appendInput('1')\">1</div>";
  page += "<div class=\"key\" onclick=\"appendInput('2')\">2</div>";
  page += "<div class=\"key\" onclick=\"appendInput('3')\">3</div>";
  page += "<div class=\"key\" onclick=\"appendInput('-')\">-</div>";
  page += "<div class=\"key\" onclick=\"appendInput('0')\">0</div>";
  page += "<div class=\"key\" onclick=\"appendInput('.')\">.</div>";
  page += "<div class=\"clear key\" onclick=\"clearInput()\">AC</div>"; // All clear button
  page += "<div class=\"delete key\" onclick=\"deleteLast()\">DEL</div>"; // Delete button
  page += "<div class=\"key\" onclick=\"sendExpression()\">=</div>";
  page += "<div class=\"key\" onclick=\"appendInput('+')\">+</div>";
  page += "</div></div>";
  page += "<script>function appendInput(value) {document.getElementById('screen').innerText += value;}function clearInput() {document.getElementById('screen').innerText = '';}function deleteLast() {var screen = document.getElementById('screen'); screen.innerText = screen.innerText.slice(0, -1);}function sendExpression() {var expression = document.getElementById('screen').innerText; var xhr = new XMLHttpRequest(); xhr.open('POST', '/calculate', true); xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded'); xhr.onreadystatechange = function() { if (xhr.readyState == 4 && xhr.status == 200) { document.getElementById('screen').innerText = xhr.responseText; } }; xhr.send('expression=' + encodeURIComponent(expression));}</script>";
  page += "</body></html>";
  server.send(200, "text/html", page);
}


void handleCalculate() {
  String expression = server.arg("expression");
  
  // Evaluate the expression
  float result = evaluateExpression(expression);
  
  // Convert the result to string
  String resultStr;
  if (!isnan(result)) {
    resultStr = String(result);
  } else {
    resultStr = "Error";
  }
  
  // Send the result back to the client
  server.send(200, "text/plain", resultStr);
}

// Function to evaluate the expression
float evaluateExpression(String expression) {
  int index = 0;
  float num1 = parseOperand(expression, index);
  char op = expression[index++];
  float num2 = parseOperand(expression, index);
  
  switch(op) {
    case '+':
      return num1 + num2;
    case '-':
      return num1 - num2;
    case '*':
      return num1 * num2;
    case '/':
      if (num2 != 0)
        return num1 / num2;
      else
        return NAN; // Handle division by zero
    default:
      return NAN; // Invalid operator
  }
}

// Function to parse operand from expression
float parseOperand(String expression, int& index) {
  String numStr = "";
  char currentChar;

  while (index < expression.length()) {
    currentChar = expression[index];
    if ((currentChar >= '0' && currentChar <= '9') || currentChar == '.') {
      numStr += currentChar;
      index++;
    } else if (currentChar == '-' && (index == 0 || expression[index - 1] == '+' || expression[index - 1] == '-' || expression[index - 1] == '*' || expression[index - 1] == '/')) {
      numStr += currentChar; // Negative sign belongs to the number
      index++;
    } else {
      break; // Reached end of operand
    }
  }

  float operand = numStr.toFloat();
  return operand;
}

void setup() {
  Serial.begin(115200);
  delay(1000); // Delay to allow serial monitor to initialize

  Serial.println("Booting...");

  WiFi.softAP(ssid, password);
  delay(100); // Allow time for the access point to start

  Serial.println("WiFi Access Point started");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", HTTP_GET, handleRoot);
  server.on("/calculate", HTTP_POST, handleCalculate);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}


