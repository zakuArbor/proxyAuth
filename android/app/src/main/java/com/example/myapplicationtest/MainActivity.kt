package com.example.myapplicationtest


//import sun.text.normalizer.UTF16.append

import android.app.Activity
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.content.Context
import android.content.Intent
import android.os.Bundle
import android.util.Log
import android.widget.AdapterView
import android.widget.ArrayAdapter
import androidx.appcompat.app.AppCompatActivity
import kotlinx.android.synthetic.main.activity_main.*
import org.jetbrains.anko.toast
import java.io.File
import java.io.FileInputStream

import com.example.myapplicationtest.R.layout

//import com.example.myapplicationtest.R.layout


class MainActivity : AppCompatActivity() {

    private var m_bluetoothAdapter: BluetoothAdapter? = null
    private lateinit var m_pairedDevices: Set<BluetoothDevice>
    //readonly
    private val REQUEST_ENABLE_BLUETOOTH = 1
    private val filename = "rem_devices.txt"

    companion object {
        val EXTRA_ADDRESS: String = "Device_address"
        val DEVICE_NAME: String = "Device_name"
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(layout.activity_main)


        if(filename.isEmpty()){
            this.filesDir
        } else {
            val directory = this.getDir("files", Context.MODE_PRIVATE)
            //this.getFileStreamPath()
            Log.i("path", ""+ directory)
            ///data/user/0/com.example.myapplicationtest/app_rem_devices

        }

        val file2 =
            File(this.filesDir.toString() + File.separator.toString() + "rem_devices.txt")
        //file2.delete()
        if(!file2.exists()){
            file2.createNewFile()

        }

        //get the default adaptor
        m_bluetoothAdapter = BluetoothAdapter.getDefaultAdapter()

        if(m_bluetoothAdapter == null){
            toast("Device does not support bluetooth")
            return
        }

        // means m_bluetooth is not going to be null
        // enabling the bluetooth
        if(!m_bluetoothAdapter!!.isEnabled){
            val enableBluetoothIntent = Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE)
            startActivityForResult(enableBluetoothIntent, REQUEST_ENABLE_BLUETOOTH)
        }





        //Log.i("device", ""+files)

        //Once the refresh button is clicked, pairedDeviceList function is called
        button_change.setOnClickListener{ pairedDeviceList()}
    }

    private fun pairedDeviceList() {
        //paired devices query
        m_pairedDevices = m_bluetoothAdapter!!.bondedDevices
        val list : ArrayList<BluetoothDevice> = ArrayList ()

        // if there's any data in it
        if (m_pairedDevices.isNotEmpty()){

            //--------------------------------------------
            //val rem_address = this.openFileInput(filename).bufferedReader().readLine()
            //val filename = "rem_devices.txt"
            val file2 =
                File(this.filesDir.toString() + File.separator.toString() + "rem_devices.txt")
            val fis = FileInputStream(file2/*openFileInput(filename)*/)
            //val scanner = Scanner(fis)
            //scanner.useDelimiter("\\Z")
            val str = "1"
            val buffer = str.toByteArray()
            val fileContent = StringBuffer("")

            //val remAddress = fis.readBytes().toString()
            val remAddress = fis.readBytes().toString(Charsets.UTF_8)

            Log.i("path", "CONNECT TO ME INPUTSTREAM "+remAddress)
            fis.close()
            ///data/user/0/com.example.myapplicationtest/files/app_rem_devices: open failed: ENOENT (No such file or directory)

            for (device: BluetoothDevice in m_pairedDevices){
                list.add(device)
                Log.i("device", ""+device)
                Log.i("device", "Address = "+device.address)

                if(remAddress == device.address){

                    val intent = Intent(this, ControlActivity::class.java)
                    intent.putExtra(EXTRA_ADDRESS, device.address)
                    intent.putExtra(DEVICE_NAME, device.name)
                    startActivity(intent)
                }

            }
        } else {
            //device is empty
            toast("no paired devices found")
        }

        val adapter = ArrayAdapter( this, android.R.layout.simple_list_item_1, list)
        list_view.adapter = adapter
        list_view.onItemClickListener = AdapterView.OnItemClickListener{_, _, position, _ ->
            val device: BluetoothDevice = list[position]

            val address: String = device.address.toString()
            //------------------------------------------
            val filename = "rem_devices.txt"

            val file2 =
                File(this.filesDir.toString() + File.separator.toString() + "rem_devices.txt")
            file2.delete()
            file2.createNewFile()



            Log.i("path", "CONNECT TO ME OUTSTREAM "+address)
            val bArray = address.toByteArray(Charsets.UTF_8)
            for (b in bArray){
                Log.i("path", "CONNECT TO ME OUTSTREAM "+b.toChar())
            }
            //val bytes: ByteArray = address.getBytes("UTF-8")
            this.openFileOutput(filename, Context.MODE_PRIVATE).use {

                it.write(bArray)
                it.flush()
                it.close()
            }

            Log.i("path", "CONNECT TO ME OUTSTREAM "+bArray.toString(Charsets.UTF_8))


            val name: String = device.name


            val intent = Intent(this, ControlActivity::class.java)
            intent.putExtra(EXTRA_ADDRESS, address)
            intent.putExtra(DEVICE_NAME, name)

            startActivity(intent)




        }


    }

    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
        super.onActivityResult(requestCode, resultCode, data)
        if (requestCode == REQUEST_ENABLE_BLUETOOTH){
            if (resultCode == Activity.RESULT_OK){
                if(m_bluetoothAdapter!!.isEnabled){
                    toast("Bluetooth has been enabled")
                } else {
                    toast("Bluetooth has been disabled")
                }

            } else if (resultCode == Activity.RESULT_CANCELED) {
                // bluetooth enabling cancelled
                toast("Bluetooth enabling has been cancelled")
            }
        }
    }

}


