package com.example.proxyauth.ui.home

import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.content.Context
import android.content.Intent
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.*
import androidx.fragment.app.Fragment
import androidx.fragment.app.FragmentActivity
import androidx.lifecycle.Observer
import androidx.lifecycle.ViewModelProvider
import androidx.navigation.Navigation
import com.example.proxyauth.R
import com.example.proxyauth.databinding.FragmentHomeBinding
import java.lang.reflect.Method

class HomeFragment : Fragment() {

    private lateinit var homeViewModel: HomeViewModel
    private var _binding: FragmentHomeBinding? = null

    private var bluetoothAdapter: BluetoothAdapter? = null

    var devAdapter : ArrayAdapter<String>? = null
    private val deviceNames : ArrayList<String> = ArrayList<String>()
    private var deviceList :ArrayList<BluetoothDevice> = ArrayList<BluetoothDevice>()
    private val REQUEST_ENABLE_BT: Int = 3

    // This property is only valid between onCreateView and
    // onDestroyView.
    private val binding get() = _binding!!

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        bluetoothAdapter = BluetoothAdapter.getDefaultAdapter()

        if (bluetoothAdapter == null) {
            activity?.let {
                Toast.makeText(it, "Bluetooth is not available", Toast.LENGTH_LONG).show()
            }
        }
    }

    override fun onStart() {
        super.onStart()

        if (!bluetoothAdapter?.isEnabled!!) {
            var enableIntent: Intent = Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE)
            startActivityForResult(enableIntent, REQUEST_ENABLE_BT)
        }
    }

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        homeViewModel =
            ViewModelProvider(this).get(HomeViewModel::class.java)

        _binding = FragmentHomeBinding.inflate(inflater, container, false)
        val root: View = binding.root


        var devListView : ListView = binding.deviceList
        devAdapter = activity?.let {
            ArrayAdapter<String>(
                it,
                android.R.layout.simple_list_item_1,
                deviceNames
            )
        }
        devListView.adapter = devAdapter
        devListView.onItemClickListener = AdapterView.OnItemClickListener{view, _, pos, _ ->
            activity?.let {
                //device.address
                //device.name
                Navigation.findNavController(view).navigate(R.id.action_navigation_home_to_navigation_device)
                //val is_connected: Boolean = isConnected(deviceList[pos])

            }
        }
        updateDevList();

        var buttonView: Button = binding.refreshBtn
        buttonView.setOnClickListener{updateDevList()}

        return root
    }

    private fun isConnected(device: BluetoothDevice): Boolean {
        return try {
            val m: Method = device.javaClass.getMethod("isConnected")
            m.invoke(device) as Boolean
        } catch (e: Exception) {
            throw IllegalStateException(e)
        }
    }

    private fun updateDevList() {
        deviceNames.clear()
        deviceList = homeViewModel.fetchDeviceList(bluetoothAdapter);
        for (device: BluetoothDevice in deviceList) {
            deviceNames.add(device.name)
        }
        devAdapter?.notifyDataSetChanged()
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }
}