//tabs=4
// --------------------------------------------------------------------------------
// TODO fill in this information for your driver, then remove this line!
//
// ASCOM Rotator driver for TCN
//
// Description:	Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam 
//				nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam 
//				erat, sed diam voluptua. At vero eos et accusam et justo duo 
//				dolores et ea rebum. Stet clita kasd gubergren, no sea takimata 
//				sanctus est Lorem ipsum dolor sit amet.
//
// Implements:	ASCOM Rotator interface version: <To be completed by driver developer>
// Author:		(XXX) Your N. Here <your@email.here>
//
// Edit Log:
//
// Date			Who	Vers	Description
// -----------	---	-----	-------------------------------------------------------
// dd-mmm-yyyy	XXX	6.0.0	Initial edit, created from ASCOM driver template
// --------------------------------------------------------------------------------
//


// This is used to define code in the template that is specific to one class implementation
// unused code can be deleted and this definition removed.
#define Rotator

using ASCOM;
using ASCOM.Astrometry;
using ASCOM.Astrometry.AstroUtils;
using ASCOM.DeviceInterface;
using ASCOM.Utilities;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;
using System.Runtime.InteropServices;
using System.Text;

namespace ASCOM.TCN
{
    //
    // Your driver's DeviceID is ASCOM.TCN.Rotator
    //
    // The Guid attribute sets the CLSID for ASCOM.TCN.Rotator
    // The ClassInterface/None attribute prevents an empty interface called
    // _TCN from being created and used as the [default] interface
    //
    // TODO Replace the not implemented exceptions with code to implement the function or
    // throw the appropriate ASCOM exception.
    //

    /// <summary>
    /// ASCOM Rotator Driver for TCN.
    /// </summary>
    [Guid("c17170ea-578d-4c4d-8857-0a4b3a9d2f73")]
    [ClassInterface(ClassInterfaceType.None)]
    public class Rotator : IRotatorV3
    {
        /// <summary>
        /// ASCOM DeviceID (COM ProgID) for this driver.
        /// The DeviceID is used by ASCOM applications to load the driver at runtime.
        /// </summary>
        internal static string driverID = "ASCOM.TCN.Rotator";
        // TODO Change the descriptive string for your driver then remove this line
        /// <summary>
        /// Driver description that displays in the ASCOM Chooser.
        /// </summary>
        private static string driverDescription = "ASCOM TCN Driver for Rotator";

        internal static string comPortProfileName = "COM Port"; // Constants used for Profile persistence
        internal static string comPortDefault = "COM1";
        internal static string traceStateProfileName = "Trace Level";
        internal static string traceStateDefault = "false";

        internal static string comPort; // Variables to hold the current device configuration
        internal static bool traceState;
        private Serial serialPort;
        private object serLock = new object();

        /// <summary>
        /// Private variable to hold the connected state
        /// </summary>
        private bool connectedState;

        /// <summary>
        /// Private variable to hold an ASCOM Utilities object
        /// </summary>
        private Util utilities;

        /// <summary>
        /// Private variable to hold an ASCOM AstroUtilities object to provide the Range method
        /// </summary>
        private AstroUtils astroUtilities;

        /// <summary>
        /// Variable to hold the trace logger object (creates a diagnostic log file with information that you specify)
        /// </summary>
        internal TraceLogger tl;

        

        //0-Disconnected 1-Connecting 2-Connected
        internal int ConnectStatus;

        RotatorDevice _rotatorDevice = new RotatorDevice();

        /// <summary>
        /// Initializes a new instance of the <see cref="TCN"/> class.
        /// Must be public for COM registration.
        /// </summary>
        public Rotator()
        {
            tl = new TraceLogger("", "TCN");
            ReadProfile(); // Read device configuration from the ASCOM Profile store

            tl.LogMessage("Rotator", "Starting initialisation");

            connectedState = false; // Initialise connected to false
            utilities = new Util(); //Initialise util object
            astroUtilities = new AstroUtils(); // Initialise astro-utilities object
            //TODO: Implement your additional construction here

            tl.LogMessage("Rotator", "Completed initialisation");

            serialPort = new Serial();

            ConnectStatus = 0;

            tl.LogMessage("Rotator", "Completed initialisation");
        }


        //
        // PUBLIC COM INTERFACE IRotatorV3 IMPLEMENTATION
        //

        #region Common properties and methods.

        /// <summary>
        /// Displays the Setup Dialog form.
        /// If the user clicks the OK button to dismiss the form, then
        /// the new settings are saved, otherwise the old values are reloaded.
        /// THIS IS THE ONLY PLACE WHERE SHOWING USER INTERFACE IS ALLOWED!
        /// </summary>
        public void SetupDialog()
        {
            // consider only showing the setup dialog if not connected
            // or call a different dialog if connected
            if (IsConnected)
                System.Windows.Forms.MessageBox.Show("Already connected, just press OK");

            using (SetupDialogForm F = new SetupDialogForm(tl))
            {
                var result = F.ShowDialog();
                if (result == System.Windows.Forms.DialogResult.OK)
                {
                    WriteProfile(); // Persist device configuration values to the ASCOM Profile store
                }
            }
        }

        public ArrayList SupportedActions
        {
            get
            {
                tl.LogMessage("SupportedActions Get", "Returning empty arraylist");
                return new ArrayList();
            }
        }

        public string Action(string actionName, string actionParameters)
        {
            LogMessage("", "Action {0}, parameters {1} not implemented", actionName, actionParameters);
            throw new ASCOM.ActionNotImplementedException("Action " + actionName + " is not implemented by this driver");
        }

        public void CommandBlind(string command, bool raw)
        {
            CheckConnected("CommandBlind");
            // TODO The optional CommandBlind method should either be implemented OR throw a MethodNotImplementedException
            // If implemented, CommandBlind must send the supplied command to the mount and return immediately without waiting for a response

            throw new ASCOM.MethodNotImplementedException("CommandBlind");
        }

        public bool CommandBool(string command, bool raw)
        {
            CheckConnected("CommandBool");
            // TODO The optional CommandBool method should either be implemented OR throw a MethodNotImplementedException
            // If implemented, CommandBool must send the supplied command to the mount, wait for a response and parse this to return a True or False value

            // string retString = CommandString(command, raw); // Send the command and wait for the response
            // bool retBool = XXXXXXXXXXXXX; // Parse the returned string and create a boolean True / False value
            // return retBool; // Return the boolean value to the client

            throw new ASCOM.MethodNotImplementedException("CommandBool");
        }

        public string CommandString(string command, bool raw)
        {
            CheckConnected("CommandString");
            // TODO The optional CommandString method should either be implemented OR throw a MethodNotImplementedException
            // If implemented, CommandString must send the supplied command to the mount and wait for a response before returning this to the client

            throw new ASCOM.MethodNotImplementedException("CommandString");
        }

        public void Dispose()
        {
            lock (serLock)
            {
                if (serialPort.Connected)
                {
                    try
                    {
                        serialPort.Connected = false;
                    }
                    catch { }
                }
                serialPort.Dispose();
            }
            serLock = null;
            // Clean up the trace logger and util objects
            tl.Enabled = false;
            tl.Dispose();
            tl = null;
            utilities.Dispose();
            utilities = null;
            astroUtilities.Dispose();
            astroUtilities = null;
        }

        public bool Connected
        {
            get
            {
                tl.LogMessage("Connected Get", IsConnected.ToString());
                return IsConnected;
            }
            set
            {
                tl.LogMessage("Connected Set", value.ToString());
                if (value == IsConnected)
                    return;

                if (value)
                {
                    tl.LogMessage("Connected Set", "Connecting to port " + comPort);
                    ConnectStatus = 1;

                    lock (serLock)
                    {
                        //Set up serial port options and try opening the port
                        serialPort.PortName = comPort;
                        serialPort.Speed = SerialSpeed.ps115200;
                        serialPort.Parity = SerialParity.None;
                        serialPort.DataBits = 8;
                        serialPort.StopBits = SerialStopBits.One;
                        serialPort.Handshake = SerialHandshake.None;
                        serialPort.ReceiveTimeoutMs = 2000;

                        try
                        {
                            serialPort.Connected = true;
                        }
                        catch (Exception e)
                        {
                            ConnectStatus = 0;
                            throw new ASCOM.DriverException("Could not open COM port: " + comPort, e);
                        }
                    }

                    //Get the controller's configuration
                    byte[] recData, serData;
                    serData = new byte[1];

                    serData[0] = 0x05;

                    connectedState = true;

                    try
                    {
                        recData = SendCommand(serData, 6);
                    }
                    catch
                    {
                        lock (serLock)
                        {
                            connectedState = false;
                            if (serialPort.Connected)
                            {
                                try
                                {
                                    serialPort.Connected = false;
                                }
                                catch { }
                            }
                        }

                        ConnectStatus = 0;
                        throw;
                    }
                    /*
                    byte[] intBytes;
                    intBytes = new byte[2];

                    //Extract the current temp coefficient
                    intBytes[0] = recData[1];
                    intBytes[1] = recData[2];
                    if (!BitConverter.IsLittleEndian) Array.Reverse(intBytes);

                    Int16 CurrTempCoeff;
                    CurrTempCoeff = BitConverter.ToInt16(intBytes, 0);

                    

                    //Extract the motor parameters
                    IdleOff = false;
                    if (recData[3] != 0) IdleOff = true;

                    Accel = Convert.ToDouble(recData[4]) / AccelScale;

                    intBytes[0] = recData[5];
                    intBytes[1] = recData[6];
                    if (!BitConverter.IsLittleEndian) Array.Reverse(intBytes);

                    MaxVel = Convert.ToDouble(BitConverter.ToUInt16(intBytes, 0)) / VelScale;

                    if (Home) FindHome();
                    */
                    ConnectStatus = 2;
                }
                else
                {
                    connectedState = false;
                    tl.LogMessage("Connected Set", "Disconnecting from port " + comPort);

                    lock (serLock)
                    {
                        if (serialPort.Connected) serialPort.Connected = false;
                    }

                    ConnectStatus = 0;
                }
            }
        }

        public string Description
        {
            // TODO customise this device description
            get
            {
                tl.LogMessage("Description Get", driverDescription);
                return driverDescription;
            }
        }

        public string DriverInfo
        {
            get
            {
                Version version = System.Reflection.Assembly.GetExecutingAssembly().GetName().Version;
                // TODO customise this driver description
                string driverInfo = "Information about the driver itself. Version: " + String.Format(CultureInfo.InvariantCulture, "{0}.{1}", version.Major, version.Minor);
                tl.LogMessage("DriverInfo Get", driverInfo);
                return driverInfo;
            }
        }

        public string DriverVersion
        {
            get
            {
                Version version = System.Reflection.Assembly.GetExecutingAssembly().GetName().Version;
                string driverVersion = String.Format(CultureInfo.InvariantCulture, "{0}.{1}", version.Major, version.Minor);
                tl.LogMessage("DriverVersion Get", driverVersion);
                return driverVersion;
            }
        }

        public short InterfaceVersion
        {
            // set by the driver wizard
            get
            {
                LogMessage("InterfaceVersion Get", "3");
                return Convert.ToInt16("3");
            }
        }

        public string Name
        {
            get
            {
                string name = "Short driver name - please customise";
                tl.LogMessage("Name Get", name);
                return name;
            }
        }

        #endregion

        #region IRotator Implementation

        private float rotatorPosition = 0; // Synced or mechanical position angle of the rotator
        private float mechanicalPosition = 0; // Mechanical position angle of the rotator

        public bool CanReverse
        {
            get
            {
                tl.LogMessage("CanReverse Get", false.ToString());
                return _rotatorDevice.CanReverce;
                //return false;
            }
        }

        public void Halt()
        {
            tl.LogMessage("Halt", "Not implemented");
            throw new ASCOM.MethodNotImplementedException("Halt");
        }

        public bool IsMoving
        {
            get
            {
                tl.LogMessage("IsMoving Get", false.ToString()); // This rotator has instantaneous movement
                return false;
            }
        }

        public void Move(float Position)
        {
            tl.LogMessage("Move", Position.ToString()); // Move by this amount
            rotatorPosition += Position;
            rotatorPosition = (float)astroUtilities.Range(rotatorPosition, 0.0, true, 360.0, false); // Ensure value is in the range 0.0..359.9999...
        }

        public void MoveAbsolute(float Position)
        {
            tl.LogMessage("MoveAbsolute", Position.ToString()); // Move to this position
            rotatorPosition = Position;
            rotatorPosition = (float)astroUtilities.Range(rotatorPosition, 0.0, true, 360.0, false); // Ensure value is in the range 0.0..359.9999...
        }

        public float Position
        {
            get
            {
                tl.LogMessage("Position Get", rotatorPosition.ToString()); // This rotator has instantaneous movement
                return rotatorPosition;
            }
        }

        public bool Reverse
        {
            get
            {
                tl.LogMessage("Reverse Get", "Not implemented");
                throw new ASCOM.PropertyNotImplementedException("Reverse", false);
            }
            set
            {
                tl.LogMessage("Reverse Set", "Not implemented");
                throw new ASCOM.PropertyNotImplementedException("Reverse", true);
            }
        }

        public float StepSize
        {
            get
            {
                tl.LogMessage("StepSize Get", "Not implemented");
                throw new ASCOM.PropertyNotImplementedException("StepSize", false);
            }
        }

        public float TargetPosition
        {
            get
            {
                tl.LogMessage("TargetPosition Get", rotatorPosition.ToString()); // This rotator has instantaneous movement
                return rotatorPosition;
            }
        }

        // IRotatorV3 methods

        public float MechanicalPosition
        {
            get
            {
                tl.LogMessage("MechanicalPosition Get", mechanicalPosition.ToString());
                return mechanicalPosition;
            }
        }

        public void MoveMechanical(float Position)
        {
            tl.LogMessage("MoveMechanical", Position.ToString()); // Move to this position

            // TODO: Implement correct sync behaviour. i.e. if the rotator has been synced the mechanical and rotator positions won't be the same
            mechanicalPosition = (float)astroUtilities.Range(Position, 0.0, true, 360.0, false); // Ensure value is in the range 0.0..359.9999...
            rotatorPosition = (float)astroUtilities.Range(Position, 0.0, true, 360.0, false); // Ensure value is in the range 0.0..359.9999...
        }

        public void Sync(float Position)
        {
            tl.LogMessage("Sync", Position.ToString()); // Sync to this position

            // TODO: Implement correct sync behaviour. i.e. the rotator mechanical and rotator positions may not be the same
            rotatorPosition = (float)astroUtilities.Range(Position, 0.0, true, 360.0, false); // Ensure value is in the range 0.0..359.9999...
        }

        #endregion

        #region Serial Methods
        /// <summary>
        /// Handles sending a command and receiving the response
        /// </summary>
        private byte[] SendCommand(byte[] serData, int expectBytes)
        {
            //Just give up if we don't even think we're connected
            if (connectedState == false) throw new ASCOM.NotConnectedException("Not connected.");

            //Try to take the serial port
            lock (serLock)
            {
                //If the port is not already open, release our mutex and error out
                if (!serialPort.Connected)
                {
                    throw new ASCOM.NotConnectedException("Not connected");
                }

                //Try to send/receive and pass any exceptions upstream
                int recBytes = 1;
                int tryCount = 0;
                const int retries = 2;
                byte cmdByte = new byte();
                byte[] response = null;

                while (true)
                {
                    tryCount++;

                    //Try to send the command and data
                    try
                    {
                        serialPort.ClearBuffers();
                        serialPort.TransmitBinary(serData);
                    }
                    catch (Exception e)
                    {
                        throw new ASCOM.DriverException("Serial port error while sending command.", e);
                    }

                    //Lot what we sent if logging is on
                    if (traceState)
                    {
                        string fmtSent = ByteArrayString(serData);

                        tl.LogMessage("SendCommand", "Sent: " + fmtSent);
                    }

                    //Try to receive the command byte of the response
                    try
                    {
                        cmdByte = serialPort.ReceiveByte();
                    }
                    catch (Exception e)
                    {
                        if (traceState) tl.LogMessage("SendCommand", "Error: Receive command byte timed out.");

                        if (tryCount < retries) continue;
                        if (traceState) tl.LogMessage("SendCommand", "Error: Giving up.");

                        throw new ASCOM.DriverException("Error receiving response command.", e);
                    }

                    //Figure out what command was returned and how many data bytes to expect
                    int responseBytes = (byte)((cmdByte >> 4) & 0x0F);

                    response = new byte[responseBytes + 1];
                    response[0] = cmdByte;

                    cmdByte = (byte)(cmdByte & 0x0F);

                    //Try to receive data bytes
                    while (recBytes < responseBytes + 1)
                    {
                        try
                        {
                            response[recBytes] = serialPort.ReceiveByte();
                            recBytes++;
                        }
                        catch (Exception e)
                        {
                            string fmtData;
                            if (response != null)
                            {
                                fmtData = ByteArrayString(response);
                            }
                            else
                            {
                                fmtData = "Nothing";
                            }

                            throw new ASCOM.DriverException("Error receiving response data, received response: " + fmtData, e);
                        }
                    }

                    //Log what we received if logging is on
                    if (traceState)
                    {
                        string fmtData = "";
                        for (int i = 0; i < response.Length; i++)
                        {
                            fmtData = fmtData + response[i].ToString("X2") + " ";
                        }

                        tl.LogMessage("SendCommand", "Recd: " + fmtData);
                    }

                    //Received a command timeout response
                    if (cmdByte == 0x0)
                    {
                        if (traceState) tl.LogMessage("SendCommand", "Error: Received command timed out.");

                        if (tryCount < retries) continue;
                        if (traceState) tl.LogMessage("SendCommand", "Error: Giving up");

                        string fmtSent = ByteArrayString(serData);
                        string fmtData = ByteArrayString(response);

                        throw new ASCOM.DriverException("Command timed out," +
                            "\n" + fmtSent + " - Sent" +
                            "\n" + fmtData + " - Received");
                    }

                    //Received an unknown command response
                    if (cmdByte == 0xF)
                    {
                        if (traceState) tl.LogMessage("SendCommand", "Error: Received unknown command.");

                        if (tryCount < retries) continue;
                        if (traceState) tl.LogMessage("SendCommand", "Error: Giving up");

                        throw new ASCOM.DriverException("Controller did not recognize command.");
                    }

                    //Validate command byte
                    if (cmdByte != (serData[0] & 0x0F))
                    {
                        if (traceState) tl.LogMessage("SendCommand", "Error: Received unmatched command.");

                        if (tryCount < retries) continue;
                        if (traceState) tl.LogMessage("SendCommand", "Error: Giving up");

                        string fmtSent = ByteArrayString(serData);
                        string fmtData = ByteArrayString(response);

                        throw new ASCOM.DriverException("Unmatched command in response. " +
                            "\n" + fmtSent + " - Sent" +
                            "\n" + fmtData + " - Received");
                    }

                    //Validate response length
                    if (responseBytes != expectBytes)
                    {
                        if (traceState) tl.LogMessage("SendCommand", "Error: Received incorrect number of bytes.");

                        if (tryCount < retries) continue;
                        if (traceState) tl.LogMessage("SendCommand", "Error: Giving up");

                        string fmtSent = ByteArrayString(serData);
                        string fmtData = ByteArrayString(response);

                        throw new ASCOM.DriverException("Received incorrect number of bytes. " +
                            "\n" + fmtSent + " - Sent" +
                            "\n" + fmtData + " - Received");
                    }

                    return response;
                }
            }
        }

        /// <summary>
        /// Format a byte array as a string of space separated hex values
        /// </summary>
        private string ByteArrayString(byte[] Data)
        {
            string fmtData = "";
            for (int i = 0; i < Data.Length; i++)
            {
                fmtData += Data[i].ToString("X2");
                if (i < Data.Length) fmtData += " ";
            }

            return fmtData;
        }
        #endregion

        #region Private properties and methods
        // here are some useful properties and methods that can be used as required
        // to help with driver development

        #region ASCOM Registration

        // Register or unregister driver for ASCOM. This is harmless if already
        // registered or unregistered. 
        //
        /// <summary>
        /// Register or unregister the driver with the ASCOM Platform.
        /// This is harmless if the driver is already registered/unregistered.
        /// </summary>
        /// <param name="bRegister">If <c>true</c>, registers the driver, otherwise unregisters it.</param>
        private static void RegUnregASCOM(bool bRegister)
        {
            using (var P = new ASCOM.Utilities.Profile())
            {
                P.DeviceType = "Rotator";
                if (bRegister)
                {
                    P.Register(driverID, driverDescription);
                }
                else
                {
                    P.Unregister(driverID);
                }
            }
        }

        /// <summary>
        /// This function registers the driver with the ASCOM Chooser and
        /// is called automatically whenever this class is registered for COM Interop.
        /// </summary>
        /// <param name="t">Type of the class being registered, not used.</param>
        /// <remarks>
        /// This method typically runs in two distinct situations:
        /// <list type="numbered">
        /// <item>
        /// In Visual Studio, when the project is successfully built.
        /// For this to work correctly, the option <c>Register for COM Interop</c>
        /// must be enabled in the project settings.
        /// </item>
        /// <item>During setup, when the installer registers the assembly for COM Interop.</item>
        /// </list>
        /// This technique should mean that it is never necessary to manually register a driver with ASCOM.
        /// </remarks>
        [ComRegisterFunction]
        public static void RegisterASCOM(Type t)
        {
            RegUnregASCOM(true);
        }

        /// <summary>
        /// This function unregisters the driver from the ASCOM Chooser and
        /// is called automatically whenever this class is unregistered from COM Interop.
        /// </summary>
        /// <param name="t">Type of the class being registered, not used.</param>
        /// <remarks>
        /// This method typically runs in two distinct situations:
        /// <list type="numbered">
        /// <item>
        /// In Visual Studio, when the project is cleaned or prior to rebuilding.
        /// For this to work correctly, the option <c>Register for COM Interop</c>
        /// must be enabled in the project settings.
        /// </item>
        /// <item>During uninstall, when the installer unregisters the assembly from COM Interop.</item>
        /// </list>
        /// This technique should mean that it is never necessary to manually unregister a driver from ASCOM.
        /// </remarks>
        [ComUnregisterFunction]
        public static void UnregisterASCOM(Type t)
        {
            RegUnregASCOM(false);
        }

        #endregion

        /// <summary>
        /// Returns true if there is a valid connection to the driver hardware
        /// </summary>
        private bool IsConnected
        {
            get
            {
                // TODO check that the driver hardware connection exists and is connected to the hardware
                return connectedState;
            }
        }

        /// <summary>
        /// Use this function to throw an exception if we aren't connected to the hardware
        /// </summary>
        /// <param name="message"></param>
        private void CheckConnected(string message)
        {
            if (!IsConnected)
            {
                throw new ASCOM.NotConnectedException(message);
            }
        }

        /// <summary>
        /// Read the device configuration from the ASCOM Profile store
        /// </summary>
        internal void ReadProfile()
        {
            using (Profile driverProfile = new Profile())
            {
                driverProfile.DeviceType = "Rotator";
                tl.Enabled = Convert.ToBoolean(driverProfile.GetValue(driverID, traceStateProfileName, string.Empty, traceStateDefault));
                comPort = driverProfile.GetValue(driverID, comPortProfileName, string.Empty, comPortDefault);
            }
        }

        /// <summary>
        /// Write the device configuration to the  ASCOM  Profile store
        /// </summary>
        internal void WriteProfile()
        {
            using (Profile driverProfile = new Profile())
            {
                driverProfile.DeviceType = "Rotator";
                driverProfile.WriteValue(driverID, traceStateProfileName, tl.Enabled.ToString());
                driverProfile.WriteValue(driverID, comPortProfileName, comPort.ToString());
            }
        }

        /// <summary>
        /// Log helper function that takes formatted strings and arguments
        /// </summary>
        /// <param name="identifier"></param>
        /// <param name="message"></param>
        /// <param name="args"></param>
        internal void LogMessage(string identifier, string message, params object[] args)
        {
            var msg = string.Format(message, args);
            tl.LogMessage(identifier, msg);
        }
        #endregion
    }
}
