package com.example.myapplicationtest


import android.app.Activity
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothServerSocket
import android.bluetooth.BluetoothSocket
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.graphics.Color
import android.os.Bundle
import android.util.Log
import android.view.View
import android.widget.AdapterView
import android.widget.ArrayAdapter
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.get
import com.example.myapplicationtest.R.layout
import kotlinx.android.synthetic.main.activity_main.*
import org.jetbrains.anko.toast
import java.io.IOException


class MainActivity : AppCompatActivity() {

    private var m_bluetoothAdapter: BluetoothAdapter? = null
    private lateinit var m_pairedDevices: Set<BluetoothDevice>
    //readonly
    private val REQUEST_ENABLE_BLUETOOTH = 1

    companion object {
        val EXTRA_ADDRESS: String = "Device_address"
    }


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(layout.activity_main)

        m_bluetoothAdapter = BluetoothAdapter.getDefaultAdapter()

        if(m_bluetoothAdapter == null){
            toast("Device does not support bluetooth")
            return
        }

        //!! means m_bluetooth is not going to be null
        if(!m_bluetoothAdapter!!.isEnabled){
            val enableBluetoothIntent = Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE)
            startActivityForResult(enableBluetoothIntent, REQUEST_ENABLE_BLUETOOTH)

        }

        //Once the refresh button is clicked, pairedDeviceList function is called
        button_change.setOnClickListener{ pairedDeviceList()}
    }

    private fun pairedDeviceList() {
        m_pairedDevices = m_bluetoothAdapter!!.bondedDevices
        val list : ArrayList<BluetoothDevice> = ArrayList ()

        // if there's any data in it
        if (m_pairedDevices.isNotEmpty()){
            for (device: BluetoothDevice in m_pairedDevices){
                list.add(device)
                Log.i("device", ""+device)
            }
        } else {
            //device is empty
            toast("no paired devices found")
        }

        val adapter = ArrayAdapter( this, android.R.layout.simple_list_item_1, list)
        list_view.adapter = adapter
        list_view.onItemClickListener = AdapterView.OnItemClickListener{_, _, position, _ ->
            val device: BluetoothDevice = list[position]
            val address: String = device.address

            val intent = Intent(this, ControlActivity::class.java)
            intent.putExtra(EXTRA_ADDRESS, address)
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

        /*var mDeviceList = ArrayList<String>()
    val REQUEST_ENABLE_BLUETOOTH = 1


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(layout.activity_main)

        val bluetoothAdapter: BluetoothAdapter? = BluetoothAdapter.getDefaultAdapter()
        if (bluetoothAdapter == null) {
            // Device doesn't support Bluetooth
            toast("Device does not support bluetooth")
            return
        }

        if (!bluetoothAdapter?.isEnabled) {
            val enableBtIntent = Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE)
            startActivityForResult(enableBtIntent, REQUEST_ENABLE_BLUETOOTH)
        }


        val pairedDevices: Set<BluetoothDevice>? = bluetoothAdapter?.bondedDevices
        pairedDevices?.forEach { device ->
            val deviceName = device.name
            val deviceHardwareAddress = device.address // MAC address
            mDeviceList.add(device.name + "\n" + device.address)
            Log.i(
                "BTBTBT",
                device.name + "\n" + device.address + "---------------------------PP"
            )
            list_view.adapter = ArrayAdapter<String>(
                applicationContext,
                android.R.layout.simple_list_item_1, mDeviceList
            )

        }

        // Register for broadcasts when a device is discovered.
        val filter = IntentFilter(BluetoothDevice.ACTION_FOUND)
        registerReceiver(receiver, filter)

        Log.i("BTBTBT", "made it here ----------------------------")


        list_view.setOnItemClickListener { adapterView: AdapterView<*>?,
                                           view: View?, position: Int, id: Long ->
            Log.i("BTBTBT", "BT_Device-------------------" + mDeviceList[position])
        }
        button_change.setOnClickListener {
            TB.text = "new text"
        }
    }

    override fun onDestroy() {
        unregisterReceiver(receiver)
        super.onDestroy()
    }

    // Create a BroadcastReceiver for ACTION_FOUND.
    private val receiver = object : BroadcastReceiver() {

        override fun onReceive(context: Context, intent: Intent) {
            val action: String = intent.action
            Log.i("BTBTBT", "called here ----------------------------")
            when (action) {
                BluetoothDevice.ACTION_FOUND -> {
                    // Discovery has found a device. Get the BluetoothDevice
                    // object and its info from the Intent.
                    val device: BluetoothDevice =
                        intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE)
                    //val deviceName = device.name
                    //val deviceHardwareAddress = device.address // MAC address
                    mDeviceList.add(device.name + "\n" + device.address)


                    Log.i(
                        "BTBTBT",
                        device.name + "\n" + device.address + "---------------------------DD"
                    )

                    list_view.adapter = ArrayAdapter<String>(
                        context,
                        android.R.layout.simple_list_item_1, mDeviceList
                    )


                }
            }
        }
    }*/
}


