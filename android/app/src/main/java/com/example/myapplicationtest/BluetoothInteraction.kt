package com.example.myapplicationtest

import android.app.IntentService
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
        var my_uuid: UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB")
        var bluetooth_socket: BluetoothSocket? = null
        lateinit var bluetooth_adapter: BluetoothAdapter
        lateinit var server_address: String
        lateinit var server_name: String
        private val buffer: ByteArray = ByteArray(1024) // mmBuffer store for the stream
    }

    override fun onHandleIntent(intent: Intent) {
        server_address = intent.getStringExtra(MainActivity.EXTRA_ADDRESS)
        server_name = intent.getStringExtra(MainActivity.DEVICE_NAME)

        // Do work here, based on the contents of dataString
        if (!connect()) {

        }
        runServer()
    }

    override fun onDestroy() {
        disconnect()
    }

    private fun connect() : Boolean {
        try{
            if(bluetooth_socket == null || !bluetooth_socket!!.isConnected){
                Log.d("data", "stage 1\n")
                bluetooth_adapter = BluetoothAdapter.getDefaultAdapter()
                Log.d("data", "stage 2\n")
                val device: BluetoothDevice = bluetooth_adapter.getRemoteDevice(server_address)
                Log.d("data", "stage 3\n")
                bluetooth_socket = device.createRfcommSocketToServiceRecord(my_uuid)
                Log.d("data", "stage 4\n")
                //stop discovery mode to stop devices attempting to connect to the app
                BluetoothAdapter.getDefaultAdapter().cancelDiscovery()
                Log.d("data", "stage 5\n")
                bluetooth_socket!!.connect()
                Log.d("data", "Successfully Connected")
                return true
            }
        }catch (e: IOException){
            Log.d("data", "Failed to Connect \n")
            e.printStackTrace()
            disconnect()
        }
        return false
    }

    private fun disconnect(){
        if (bluetooth_socket != null) {
            try {
                bluetooth_socket!!.outputStream.close()
                bluetooth_socket!!.inputStream.close()
                bluetooth_socket!!.close()
            } catch (e: IOException) {
            } finally { // close the socket
                bluetooth_socket = null
                Toast.makeText(this, "Disconnecting from Server", Toast.LENGTH_SHORT).show()
                val intent = Intent(this, MainActivity::class.java)
                intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
                this.startActivity(intent)
            }
        }
    }

    private fun sendCommand(input: String){
        if (bluetooth_socket == null || !bluetooth_socket!!.isConnected) {
            disconnect()
        }

        try{
            bluetooth_socket!!.outputStream?.write(input.toByteArray())
        } catch (e: IOException){
        }
    }

    private fun receiveCommand() : String {
        if (bluetooth_socket == null || !bluetooth_socket!!.isConnected) {
            disconnect()
        }

        bluetooth_socket!!.inputStream.read(buffer)
        Log.d("data", buffer.toString(Charsets.UTF_8))
        return buffer.toString(Charsets.UTF_8)
    }

    private fun runServer() {
        var buf : String

        while(bluetooth_socket != null && bluetooth_socket!!.isConnected) {
            buf = receiveCommand()
            if (buf == "" || buf.length < 0) {
                Log.d("data", "Failed to receive data")
                stopSelf()
            }
            sendCommand("World")
        }
        disconnect()
    }
}
