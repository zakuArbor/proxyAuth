package com.example.myapplicationtest


import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
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


class MainActivity : AppCompatActivity() {


    var mDeviceList = ArrayList<String>()

    
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(layout.activity_main)

        val bluetoothAdapter: BluetoothAdapter? = BluetoothAdapter.getDefaultAdapter()
        if (bluetoothAdapter == null) {
            // Device doesn't support Bluetooth
        }

        if (bluetoothAdapter?.isEnabled == false) {
            val enableBtIntent = Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE)
            startActivityForResult(enableBtIntent, 1)
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
    }
}
