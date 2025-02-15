#include <xc.h>
#include <stdio.h>
#include "LCD.h"
#include "pic18f4321-Config.h"
#define _XTAL_FREQ 4000000  // Oscillator frequency

//Bit banging bits to use
#define I2C_SDA LATCbits.LATC4
#define I2C_SCL LATCbits.LATC3
#define I2C_SDA_DIR TRISCbits.TRISC4
#define I2C_SCL_DIR TRISCbits.TRISC3
#define I2C_SDA_READ PORTCbits.RC4

//Port for DHT11 Sensor
#define SENSOR_PORT PORTBbits.RB4
#define SENSOR_DIR TRISBbits.TRISB4

//Wind Sensor Functions
void setupWindADC(void);
unsigned int readWindADC(void);

//DHT11 Functions
void initiate_sensor(void);
char fetch_byte(void);
char validate_data(char humMajor, char humMinor, char tempMajor, char tempMinor, char checkSum);
void display_readings(char humMajor, char tempMajor);

//I2C Bit Bash
void I2C_Init(void);
void I2C_Start(void);
void I2C_Stop(void);
unsigned char I2C_Write(unsigned char data);
unsigned char I2C_Read(unsigned char ack);

//Light Sensor
void BH1750_Init(void);
unsigned int BH1750_Read_Light(void);

////////////////////////////////////////////////////////////////////////////////
void main(void) {
    
    unsigned int adcValue;
    float lux;
    int windSpeedVoltage, wbgt, naturalWetBulb, globeTemp;
    char humMajor, humMinor, tempMajor, tempMinor, checksum;
    
        // Oscillator Configuration
    OSCCON = 0x60;  // Configure internal oscillator to 4 MHz
    
    // Analog Configuration
    ADCON1 = 0x0F;  // Disable ADC functionality on PORTC pins

    // Initialize peripherals
    LCD_init();
    LCD_clear();
    LCD_cursor_set(1, 1);
    LCD_write_string("LCD? Dub.");    
    __delay_ms(1000);
    
    I2C_Init();
    LCD_clear();
    LCD_cursor_set(1, 1);
    LCD_write_string("I2C_init? Dub.");    
    __delay_ms(1000);
    
    setupWindADC(); //Function call that sets up our wind sensor adc
    LCD_clear();
    LCD_cursor_set(1, 1);
    LCD_write_string("Wind? Dub.");    
    __delay_ms(1000);
    
    //InitializeBH1750
    BH1750_Init();
    LCD_clear();
    LCD_cursor_set(1, 1);
    LCD_write_string("Lux? Dub.");    
    __delay_ms(1000);

    while (1) {
        // Read light intensity from BH1750
        unsigned int rawLight = BH1750_Read_Light();
        char debugBuffer[16];
        sprintf(debugBuffer, "Raw Lux: %u", rawLight);
        LCD_clear();
        LCD_cursor_set(1, 1);
        LCD_write_string(debugBuffer);
        __delay_ms(1000);
        lux = (float)rawLight / 1.2f;

        // Read wind speed voltage from ADC
        adcValue = readWindADC(); //This is reading from the DHT11 because of the function call
        windSpeedVoltage = (adcValue * 5000) / 1023;  // Assuming VREF+ = 2V
        LCD_clear();
        LCD_cursor_set(1, 1);
        LCD_write_string("Wind comp done");    
        __delay_ms(1000);
        
        // Read DHT11 temperature and humidity
        initiate_sensor();
        while (SENSOR_PORT);    // Wait for signal to go low
        while (!SENSOR_PORT);   // Wait for signal to go high
        while (SENSOR_PORT);    // Wait for signal to go low again

        
        humMajor = fetch_byte();
        humMinor = fetch_byte();
        tempMajor = fetch_byte();
        tempMinor = fetch_byte();
        checksum = fetch_byte();

        if (!validate_data(humMajor, humMinor, tempMajor, tempMinor, checksum)) {
            LCD_clear();
            LCD_cursor_set(1, 1);
            LCD_write_string("DHT11 Error");
            __delay_ms(1000);
            continue;
        }
        
        LCD_clear();
        LCD_cursor_set(1, 1);
        LCD_write_string("Sensors dun");
        __delay_ms(500);
        // Calculate WBGT components
        naturalWetBulb = humMajor + windSpeedVoltage + (int)lux;
        globeTemp = (int)lux + windSpeedVoltage;
        wbgt = ( (10 * tempMajor) + (70 * naturalWetBulb) + (20 * globeTemp) ) / 100;

        // Display WBGT and category
        LCD_clear();
        LCD_cursor_set(1, 1);
        LCD_write_string("WBGT:");
        char buffer[16];
        sprintf(buffer, "%d C", wbgt);
        LCD_write_string(buffer);
        LCD_cursor_set(2, 1);

        if (wbgt <= 26) { // Approximately 80°F
            LCD_write_string("Clear: No action.");
        } 
        else if (wbgt <= 29) { // Approximately 84.9°F
            LCD_write_string("Low: 5m rest/25m.");
        } 
        else if (wbgt <= 31) { // Approximately 87.9°F
            LCD_write_string("Med: Breaks & Ice.");
        } 
        else if (wbgt <= 32) { // Approximately 88.9°F
            LCD_write_string("High: Ob needed.");
        } 
        else {
            LCD_write_string("Extreme: Cancel.");
        }
        __delay_ms(4000);
    }
}

////////////////////////////////////////////////////////////////////////////////
    // ADC Configuration
    void setupWindADC(void) {

    ADCON0 = 0x01;  // CHS = 0 (AN0 selected), ADON = 1 (ADC turned on)
    ADCON1 = 0x0E; // VREF- = VSS, VREF+ = External, Select AN1
    ADCON2 = 0x2A;  // ADCS = 2 (FOSC/32), ACQT = 2 (4 TAD), ADFM = 1 (Right-justified)
}

    // Read ADC
    unsigned int readWindADC(void) {
        ADCON0bits.GO = 1; // Start conversion
        while (ADCON0bits.GO_nDONE);
        return (ADRESH << 8) | ADRESL;
    }

void initiate_sensor(void) {
    SENSOR_DIR = 0;       // Set RB5 as output
    LATBbits.LATB4 = 0;   // Pull low to initiate
    __delay_ms(18);       // Minimum 18ms delay
    LATBbits.LATB4 = 1;   // Release high
    __delay_us(35);       // Wait 20-40us
    SENSOR_DIR = 1;       // Set RB5 as input to listen for sensor
}

// Function to read a single byte from the DHT11
char fetch_byte() {
    char receivedData = 0;

    for (int bitCount = 0; bitCount < 8; bitCount++) {
        while (!SENSOR_PORT);  
        __delay_us(35);        
        receivedData <<= 1;   
        if (SENSOR_PORT) {     
            receivedData |= 1;
        }
        while (SENSOR_PORT);   
    }

    return receivedData;
}

    // Validate DHT11 Data
    char validate_data(char humMajor, char humMinor, char tempMajor, char tempMinor, char checkSum) {
    return (checkSum == (humMajor + humMinor + tempMajor + tempMinor));
}

    // I2C Functions
    void I2C_Init(void) {
    I2C_SDA_DIR = 1; // Set SDA as input initially
    I2C_SCL_DIR = 1; // Set SCL as input initially
    }

void I2C_Start(void) {
    I2C_SDA_DIR = 0; // Set SDA as output
    I2C_SCL_DIR = 0; // Set SCL as output

    I2C_SDA = 1; // Make sure SDA is high
    I2C_SCL = 1; // Make sure SCL is high
    __delay_us(10);
    I2C_SDA = 0; // Pull SDA low
    __delay_us(10);
    I2C_SCL = 0; // Pull SCL low
}

void I2C_Stop(void) {
    I2C_SDA_DIR = 0; // Set SDA as output
    I2C_SDA = 0;     // Pull SDA low
    __delay_us(10);
    I2C_SCL = 1;     // Pull SCL high
    __delay_us(10);
    I2C_SDA = 1;     // Pull SDA high
    __delay_us(10);
}

unsigned char I2C_Write(unsigned char data) {
    for (int i = 0; i < 8; i++) {
        I2C_SDA = (data & 0x80) ? 1 : 0;
        __delay_us(10);
        I2C_SCL = 1;
        __delay_us(10);
        I2C_SCL = 0;
        data <<= 1;
    }
    I2C_SDA_DIR = 1;
    I2C_SCL = 1;
    __delay_us(10);
    unsigned char ack = !I2C_SDA_READ;
    I2C_SCL = 0;
    I2C_SDA_DIR = 0;

    // Debugging ACK
    char debugBuffer[16];
    sprintf(debugBuffer, "ACK:%u", ack);
    LCD_clear();
    LCD_cursor_set(1, 1);
    LCD_write_string(debugBuffer);
    __delay_ms(500);

    return ack;
}


unsigned char I2C_Read(unsigned char ack) {
    unsigned char data = 0;
    I2C_SDA_DIR = 1; // Set SDA as input

    for (int i = 0; i < 8; i++) {
        data <<= 1;
        I2C_SCL = 1; // Pulse the clock
        __delay_us(10);
        if (I2C_SDA_READ) {
            data |= 1; // Read the bit
        }
        I2C_SCL = 0;
        __delay_us(10);
    }

    // Acknowledge phase
    I2C_SDA_DIR = 0; // Set SDA as output
    I2C_SDA = ack ? 0 : 1; // Send ACK (0) or NACK (1)
    I2C_SCL = 1;
    __delay_us(10);
    I2C_SCL = 0;
    return data;
}

void BH1750_Init(void) {
    I2C_Start();
    if (!I2C_Write(0x23 << 1)) { // Send slave address with write bit
        LCD_clear();
        LCD_cursor_set(1, 1);
        LCD_write_string("Addr FAIL");
        while (1); // Halt if address fails
    }
    I2C_Write(0x01); // Power On
    I2C_Stop();
    __delay_ms(10); // Wait for stabilization

    I2C_Start();
    if (!I2C_Write(0x23 << 1)) { // Send slave address with write bit
        LCD_clear();
        LCD_cursor_set(1, 1);
        LCD_write_string("Cmd FAIL");
        while (1); // Halt if command fails
    }
    I2C_Write(0x10); // Continuous H-Resolution Mode
    I2C_Stop();
    __delay_ms(120); // Wait for the first measurement
}

unsigned int BH1750_Read_Light(void) {
    unsigned char msb = 0, lsb = 0;

    // Start I2C read sequence
    I2C_Start();
    if (!I2C_Write((0x23 << 1) | 1)) { // Send slave address with read bit
        LCD_clear();
        LCD_cursor_set(1, 1);
        LCD_write_string("Read FAIL");
        I2C_Stop();
        return 0xFFFF; // Return an error code
    }

    // Read MSB and LSB
    msb = I2C_Read(1); // Read MSB with ACK
    lsb = I2C_Read(0); // Read LSB with NACK
    I2C_Stop();

    // Debugging: Display raw MSB and LSB
    char debugBuffer[16];
    sprintf(debugBuffer, "MSB:%02X LSB:%02X", msb, lsb);
    LCD_clear();
    LCD_cursor_set(1, 1);
    LCD_write_string(debugBuffer);
    __delay_ms(1000);

    // Combine MSB and LSB into a single value
    return ((unsigned int)msb << 8) | lsb;
}

