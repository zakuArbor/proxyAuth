package com.example.myapplicationtest

import android.app.ProgressDialog
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothSocket
import android.content.Context
import android.os.AsyncTask
import android.os.Bundle
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import kotlinx.android.synthetic.main.control_layout.*
import java.io.IOException
import java.util.*

class ControlActivity: AppCompatActivity(){

    companion object{
        var m_myUUID: UUID = UUID.fromString("70eb6b56-4e02-11ea-b77f-2e728ce88125")
        var m_bluetoothSocket: BluetoothSocket? = null
        lateinit var m_progress: ProgressDialog
        lateinit var m__bluetoothAdapter: BluetoothAdapter
        var m_isConnected: Boolean = false
        lateinit var m_address: String

    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.control_layout)
        m_address = intent.getStringExtra(MainActivity.EXTRA_ADDRESS)

        ConnectToDevice(this).execute()

        control_led_on.setOnClickListener{ sendCommand("a")}
        control_led_off.setOnClickListener{ sendCommand("b")}
        control_led_disconnect.setOnClickListener{ disconnect()}

    }

    private fun sendCommand(input: String){
        if (m_bluetoothSocket != null){
            try{
                m_bluetoothSocket!!.outputStream.write(input.toByteArray())
            } catch (e: IOException){
                e.printStackTrace()
            }
        }
    }

    private fun disconnect(){
        if (m_bluetoothSocket != null){
            try {
                m_bluetoothSocket!!.close()
                m_bluetoothSocket = null
                m_isConnected = false
            } catch (e: IOException){
                e.printStackTrace()
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
            m_progress = ProgressDialog.show(context, "Connecting...", "please wait")
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
                e.printStackTrace()

            }
            return null
        }

        override  fun onPostExecute(result: String?){
            super.onPostExecute(result)
            if(!connectSuccess){
                Log.i("data", "couldn't connect")
            } else {
                m_isConnected = true
            }
            m_progress.dismiss()
        }
    }
}