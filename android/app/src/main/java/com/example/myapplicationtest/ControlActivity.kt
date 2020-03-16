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

        val cDevice = "Connected device: $m_name"
        val cDeviceAddr = "Device Address: $m_address"
        connectedDevice.text = cDevice
        deviceAddress.text = cDeviceAddr

        // connecting to the device
        ConnectToDevice(this).execute()

        // should be receiving messages from the server at all times
        // not sure if we should just call the method or

        //receiveCommand()
        test_button.setOnClickListener{ sendCommand("Hello World!")}

        control_led_disconnect.setOnClickListener{ disconnect()}

    }

    private fun sendCommand(input: String){
        if (m_bluetoothSocket != null){
            try{
                Log.d("data", input)
                m_bluetoothSocket!!.outputStream.write(input.toByteArray())
            } catch (e: IOException){
                e.printStackTrace()
            }
        }
    }

    private fun receiveCommand(){
        var numBytes: Int // bytes returned from read()

        // Keep listening to the InputStream until an exception occurs.
        while (true) {
            // Read from the InputStream.
            numBytes = try {
                m_bluetoothSocket!!.inputStream.read(mmBuffer)
            } catch (e: IOException) {
                Log.d("data", "Input stream was disconnected", e)
                break
            }
            // we have the data from the computer in the buffer mmBuffer now
            // turn it into text here
            toast("data received")
            Log.d("data", mmBuffer.toString(Charsets.UTF_8))
        }

    }

    private fun disconnect(){
        if (m_bluetoothSocket != null){
            try {
                m_bluetoothSocket!!.outputStream.close()
                m_bluetoothSocket!!.inputStream.close()
                m_bluetoothSocket!!.close()
            } catch (e: IOException){
                e.printStackTrace()
            }
            finally { //close the socket added
                m_bluetoothSocket = null
                m_isConnected = false
                toast("Disconnecting from server")
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
            m_progress = ProgressDialog.show(context, "Connecting...", "Please wait")
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

                Log.d("data", "FAILED TO CONNECT\n")

                e.printStackTrace()

            }
            return null
        }

        override  fun onPostExecute(result: String?){
            super.onPostExecute(result)
            if(!connectSuccess){
                Log.i("data", "Couldn't CONNECT")
            } else {
                Log.i("data", "CONNECTED")
                m_isConnected = true
            }
            m_progress.dismiss()
        }
    }
}