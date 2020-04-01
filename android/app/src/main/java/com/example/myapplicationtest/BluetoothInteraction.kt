package com.example.myapplicationtest

import android.app.Activity
import android.app.IntentService
import android.app.ProgressDialog
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothSocket
import android.content.Intent
import android.util.Log
import android.widget.Toast
import java.io.IOException
import java.util.*

class BluetoothInteraction : IntentService("BluetoothInteraction") {
    companion object {
        var m_myUUID: UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB")
        var m_bluetoothSocket: BluetoothSocket? = null
        lateinit var m__bluetoothAdapter: BluetoothAdapter
        var m_isConnected: Boolean = false
        lateinit var m_address: String
        lateinit var m_name: String
        private val mmBuffer: ByteArray = ByteArray(1024) // mmBuffer store for the stream
    }

    override fun onHandleIntent(intent: Intent) {
        // Gets data from the incoming Intent
        m_address = intent.getStringExtra(MainActivity.EXTRA_ADDRESS)
        m_name = intent.getStringExtra(MainActivity.DEVICE_NAME)

        // Do work here, based on the contents of dataString
        if (!connect()) {

        }
        runServer()
    }

    private fun sendCommand(input: String){
        if (m_bluetoothSocket != null){
            try{
                //Log.d("data", "DATA Incoming")
                Log.d("data", input)
                m_bluetoothSocket!!.outputStream.write(input.toByteArray())
            } catch (e: IOException){
                e.printStackTrace()
            }
        }
    }

    private fun connect() : Boolean {
        try{
            if(m_bluetoothSocket == null || !m_isConnected){
                m__bluetoothAdapter = BluetoothAdapter.getDefaultAdapter()
                val device: BluetoothDevice = m__bluetoothAdapter.getRemoteDevice(
                    m_address
                )
                m_bluetoothSocket = device.createInsecureRfcommSocketToServiceRecord(
                    m_myUUID
                )
                //stops the app for looking for other devices
                BluetoothAdapter.getDefaultAdapter().cancelDiscovery()
                m_bluetoothSocket!!.connect()
                Log.i("data", "Successfully Connected")
                m_isConnected = true
                return true
            }
        }catch (e: IOException){
            Log.d("data", "Failed to Connect \n")
            e.printStackTrace()
        }
        return false
    }

    override fun onDestroy() {
        disconnect()
    }

    private fun disconnect(){
        if (m_bluetoothSocket != null) {
            try {
                m_isConnected = false
                m_bluetoothSocket!!.outputStream.close()
                m_bluetoothSocket!!.inputStream.close()
                m_bluetoothSocket!!.close()
            } catch (e: IOException) {
                e.printStackTrace()
            } finally { // close the socket
                m_bluetoothSocket = null
                Toast.makeText(this, "Disconnecting from Server", Toast.LENGTH_SHORT).show()
                val intent = Intent(this, MainActivity::class.java)
                intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
                this.startActivity(intent)
            }
        }
    }

    private fun runServer() {
        var buf : String

        while(m_isConnected) {
            buf = receiveCommand()
            if (buf.length < 0) {
                Log.d("data", "Failed to receive data")
                stopSelf()
                //break
            }
            sendCommand("World")
        }
    }

    private fun receiveCommand() : String {
        var numBytes: Int // bytes returned from read()
        try {
            // Keep listening to the InputStream until an exception occurs.
            // Read from the InputStream.
            numBytes = m_bluetoothSocket!!.inputStream.read(mmBuffer)
            Log.d("data", numBytes.toString())
            Log.d("data", mmBuffer.toString(Charsets.UTF_8))
            Toast.makeText(this, mmBuffer.toString(Charsets.UTF_8), Toast.LENGTH_LONG)
            return mmBuffer.toString(Charsets.UTF_8)
        } catch (e: IOException) {
            Log.d("data", "Input stream was disconnected", e)
        }
        return ""
    }
}

