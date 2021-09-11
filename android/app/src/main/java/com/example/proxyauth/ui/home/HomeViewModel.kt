package com.example.proxyauth.ui.home

import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.util.Log
import androidx.lifecycle.MutableLiveData
import androidx.lifecycle.ViewModel


class HomeViewModel : ViewModel() {

    /*
    * Fetch all the devices that are trusted and connected
    */
    fun fetchDeviceList(bluetoothAdapter: BluetoothAdapter?) : ArrayList<BluetoothDevice> {
        var list: ArrayList<BluetoothDevice>  = ArrayList<BluetoothDevice>()
        list.addAll(bluetoothAdapter?.bondedDevices as Set<BluetoothDevice>)
        return list
    }
}