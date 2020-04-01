package com.example.myapplicationtest

import android.app.ProgressDialog
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothSocket
import android.content.Context
import android.content.Intent
import android.os.AsyncTask
import android.os.Bundle
import android.util.Log
import android.view.Menu
import android.view.MenuInflater
import android.view.MenuItem
import androidx.appcompat.app.AppCompatActivity
import kotlinx.android.synthetic.main.control_layout.*
import org.jetbrains.anko.toast
import java.io.IOException
import java.util.*

class ControlActivity: AppCompatActivity() {

    companion object {
        lateinit var m_address: String
        lateinit var m_name: String
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
        val intent = Intent(this, BluetoothInteraction::class.java)
        intent.putExtra(MainActivity.EXTRA_ADDRESS, m_address)
        intent.putExtra(MainActivity.DEVICE_NAME, m_name)
        intent.also { intent ->
            startService(intent)
        }

        disconnect.setOnClickListener { stopService(intent) }
    }


    override fun onCreateOptionsMenu(menu: Menu): Boolean {
        val inflater: MenuInflater = menuInflater
        inflater.inflate(R.menu.mainmenu, menu)
        return true
    }

    override fun onOptionsItemSelected(item: MenuItem): Boolean {
        // Handle item selection

        return when (item.itemId) {
            R.id.help -> {
                val intent = Intent(this, HelpActivity::class.java)
                startActivity(intent)
                true
            }
            R.id.settings -> {
                val intent = Intent(this, SettingsActivity::class.java)
                startActivity(intent)
                true
            }
            R.id.about -> {
                val intent = Intent(this, AboutActivity::class.java)
                startActivity(intent)
                true
            }
            else -> super.onOptionsItemSelected(item)
        }
    }
    /*
    Message handling helper function
    The numbers can be used based on what we decide they represent
     */
    fun handleMessage(input: String){

        val msg = input.split("|").toTypedArray()

        when(msg[0]){
            "1" -> {

            }
            "2" -> {

            }
            "3" -> {

            }
        }


    }
}
