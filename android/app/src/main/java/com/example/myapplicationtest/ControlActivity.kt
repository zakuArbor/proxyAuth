package com.example.myapplicationtest

import android.app.IntentService
import android.app.ProgressDialog
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothSocket
import android.content.Context
import android.content.Intent
import android.os.AsyncTask
import android.os.Bundle
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import androidx.core.content.ContextCompat.startActivity
import kotlinx.android.synthetic.main.control_layout.*
import org.jetbrains.anko.toast
import java.io.IOException
import java.io.OutputStream
import java.util.*

class ControlActivity: AppCompatActivity(){

    companion object{
        var m_myUUID: UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB")
        var m_bluetoothSocket: BluetoothSocket? = null
        lateinit var m_progress: ProgressDialog
        lateinit var m__bluetoothAdapter: BluetoothAdapter
        var m_isConnected: Boolean = false
        lateinit var m_address: String
        lateinit var m_name: String
        private val mmBuffer: ByteArray = ByteArray(1024) // mmBuffer store for the stream
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.control_layout)

        m_address = intent.getStringExtra(MainActivity.EXTRA_ADDRESS)
        m_name = intent.getStringExtra(MainActivity.DEVICE_NAME)

        val cDevice = "Connected Device: $m_name"
        val cDeviceAddr = "Device Address: $m_address"
        connectedDevice.text = cDevice
        deviceAddress.text = cDeviceAddr

        // connecting to the device
        ConnectToDevice(this).execute()

        test_button.setOnClickListener{ sendCommand("Hello World!")} //for now sending this
        control_led_disconnect.setOnClickListener{ disconnect()}
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
        receiveCommand()
    }

    private fun receiveCommand(){
        var numBytes: Int // bytes returned from read()
        try {
            // Keep listening to the InputStream until an exception occurs.
            while (true) {
                // Read from the InputStream.
                //numBytes =
                try {
                    numBytes = m_bluetoothSocket!!.inputStream.read(mmBuffer)
                    Log.d("data", numBytes.toString())
                    Log.d("data", mmBuffer.toString(Charsets.UTF_8))
                    toast(mmBuffer.toString(Charsets.UTF_8))
                } catch (e: IOException) {
                    Log.d("data", "Input stream was disconnected", e)
                    break
                }
                // we have the data from the computer in the buffer mmBuffer now, turn it into text here
                toast("data received")
                Log.d("data", mmBuffer.toString(Charsets.UTF_8))
            }

        } catch (e: Exception){
            toast("Failed to read")
        }
    }

    private fun disconnect(){
        if (m_bluetoothSocket != null){
            try {
                m_isConnected = false
                m_bluetoothSocket!!.outputStream.close()
                m_bluetoothSocket!!.inputStream.close()
                m_bluetoothSocket!!.close()
            } catch (e: IOException){
                e.printStackTrace()
            } finally { // close the socket
                m_bluetoothSocket = null
                toast("Disconnecting from Server")
                //m_isConnected = false
                val intent = Intent(this, MainActivity::class.java)
                startActivity(intent)
            }
        }
        finish()
    }

    private class ConnectToDevice(c: Context): AsyncTask<Void, Void, String>(){

        private var connectSuccess: Boolean = true
        private val context: Context

        init {
            this.context = c
        }

        override fun onPreExecute(){
            super.onPreExecute()
            m_progress = ProgressDialog.show(context, "Connecting...", "Please Wait")
        }

        override fun doInBackground(vararg p0: Void?): String?{
            try{
                if(m_bluetoothSocket == null || !m_isConnected){
                    m__bluetoothAdapter = BluetoothAdapter.getDefaultAdapter()
                    val device: BluetoothDevice = m__bluetoothAdapter.getRemoteDevice(m_address)
                    m_bluetoothSocket = device.createInsecureRfcommSocketToServiceRecord(m_myUUID)
                    //stops the app for looking for other devices
                    BluetoothAdapter.getDefaultAdapter().cancelDiscovery()
                    m_bluetoothSocket!!.connect()
                }
            }catch (e: IOException){
                connectSuccess =  false
                Log.d("data", "Failed to Connect \n")
                e.printStackTrace()
            }
            return null
        }

        override fun onPostExecute(result: String?){
            super.onPostExecute(result)
            if(!connectSuccess){
                Log.i("data", "Unable to Connect")
                val intent = Intent(this.context, MainActivity::class.java)
                this.context.toast("Failed to Connect to " + m_address)
                this.context.startActivity(intent)
            } else {
                Log.i("data", "Successfully Connected")
                m_isConnected = true
            }
            m_progress.dismiss()
        }
    }
}


// for disconnet(), if needed
/** finally {
    m_bluetoothSocket!!.close()
} */
