import 'package:flutter/material.dart';
import 'package:mqtt_client/mqtt_client.dart';
import 'package:mqtt_client/mqtt_server_client.dart';

void main() {
  runApp(MyApp());
}

class MyApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Smart Lighting',
      theme: ThemeData(primarySwatch: Colors.deepPurple),
      home: HomeScreen(),
      debugShowCheckedModeBanner: false,
    );
  }
}

class Light {
  final int id;
  final String name;
  final String room;
  bool isOn;

  Light({required this.id, required this.name, required this.room, this.isOn = false});
}

class HomeScreen extends StatefulWidget {
  @override
  _HomeScreenState createState() => _HomeScreenState();
}

class _HomeScreenState extends State<HomeScreen> {
  List<Light> lights = [];
  bool isConnected = false;
  MqttServerClient? client;

  final String broker = 'broker.emqx.io';
  final int port = 1883;

  @override
  void initState() {
    super.initState();
    _initializeLights();
    _connectMQTT();
  }

  void _initializeLights() {
    lights = [
      Light(id: 1, name: 'Light 1', room: 'Living Room'),
      Light(id: 2, name: 'Light 2', room: 'Bedroom'),
      Light(id: 3, name: 'Light 3', room: 'Kitchen'),
      Light(id: 4, name: 'Light 4', room: 'Study Room'),
    ];
  }

  Future<void> _connectMQTT() async {
    client = MqttServerClient(broker, 'flutter_${DateTime.now().millisecondsSinceEpoch}');
    client!.port = port;
    client!.keepAlivePeriod = 60;

    final connMessage = MqttConnectMessage()
        .withClientIdentifier('flutter_client')
        .startClean();
    client!.connectionMessage = connMessage;

    try {
      await client!.connect();
    } catch (e) {
      setState(() => isConnected = false);
      return;
    }

    if (client!.connectionStatus?.state == MqttConnectionState.connected) {
      setState(() => isConnected = true);
      _subscribeToTopics();
    }
  }

  void _subscribeToTopics() {
    for (int i = 1; i <= 4; i++) {
      client!.subscribe('home/light/$i/status', MqttQos.atLeastOnce);
    }

    client!.updates?.listen((List<MqttReceivedMessage<MqttMessage>> c) {
      final MqttPublishMessage message = c[0].payload as MqttPublishMessage;
      final payload = MqttPublishPayload.bytesToStringAsString(message.payload.message);
      final topic = c[0].topic;

      if (topic.startsWith('home/light/')) {
        final parts = topic.split('/');
        final lightId = int.parse(parts[2]);
        setState(() {
          final index = lights.indexWhere((l) => l.id == lightId);
          if (index != -1) {
            lights[index].isOn = (payload == 'ON');
          }
        });
      }
    });
  }

  void _controlLight(Light light, bool turnOn) {
    final topic = 'home/light/${light.id}/command';
    final payload = turnOn ? 'ON' : 'OFF';
    
    final builder = MqttClientPayloadBuilder();
    builder.addString(payload);
    client!.publishMessage(topic, MqttQos.atLeastOnce, builder.payload);

    setState(() {
      light.isOn = turnOn;
    });
  }

  void _controlAllLights(bool turnOn) {
    final topic = 'home/all/command';
    final payload = turnOn ? 'ON' : 'OFF';
    
    final builder = MqttClientPayloadBuilder();
    builder.addString(payload);
    client!.publishMessage(topic, MqttQos.atLeastOnce, builder.payload);

    setState(() {
      for (var light in lights) {
        light.isOn = turnOn;
      }
    });
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('Smart Lighting Control'),
        backgroundColor: Colors.deepPurple,
        actions: [
          Container(
            margin: EdgeInsets.only(right: 16),
            child: Row(
              children: [
                Icon(isConnected ? Icons.wifi : Icons.wifi_off, color: Colors.white),
                SizedBox(width: 4),
                Text(isConnected ? 'Online' : 'Offline', style: TextStyle(fontSize: 12)),
              ],
            ),
          ),
        ],
      ),
      body: Padding(
        padding: EdgeInsets.all(16),
        child: Column(
          children: [
            Card(
              child: Padding(
                padding: EdgeInsets.all(16),
                child: Row(
                  children: [
                    Expanded(
                      child: ElevatedButton.icon(
                        onPressed: () => _controlAllLights(true),
                        icon: Icon(Icons.lightbulb),
                        label: Text('ALL ON'),
                        style: ElevatedButton.styleFrom(backgroundColor: Colors.amber),
                      ),
                    ),
                    SizedBox(width: 16),
                    Expanded(
                      child: ElevatedButton.icon(
                        onPressed: () => _controlAllLights(false),
                        icon: Icon(Icons.lightbulb_outline),
                        label: Text('ALL OFF'),
                        style: ElevatedButton.styleFrom(backgroundColor: Colors.grey),
                      ),
                    ),
                  ],
                ),
              ),
            ),
            SizedBox(height: 20),
            Expanded(
              child: GridView.builder(
                gridDelegate: SliverGridDelegateWithFixedCrossAxisCount(
                  crossAxisCount: 2,
                  crossAxisSpacing: 16,
                  mainAxisSpacing: 16,
                  childAspectRatio: 0.85,
                ),
                itemCount: lights.length,
                itemBuilder: (context, index) {
                  final light = lights[index];
                  return _buildLightCard(light);
                },
              ),
            ),
          ],
        ),
      ),
    );
  }

  Widget _buildLightCard(Light light) {
    return Card(
      shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(16)),
      child: InkWell(
        onTap: () => _controlLight(light, !light.isOn),
        child: Container(
          decoration: BoxDecoration(
            gradient: LinearGradient(
              colors: light.isOn
                  ? [Colors.amber.shade300, Colors.orange.shade400]
                  : [Colors.grey.shade500, Colors.grey.shade700],
            ),
            borderRadius: BorderRadius.circular(16),
          ),
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              Icon(Icons.lightbulb, size: 50, color: Colors.white),
              SizedBox(height: 12),
              Text(light.name, style: TextStyle(fontSize: 18, fontWeight: FontWeight.bold, color: Colors.white)),
              Text(light.room, style: TextStyle(fontSize: 12, color: Colors.white70)),
              SizedBox(height: 12),
              Container(
                padding: EdgeInsets.symmetric(horizontal: 16, vertical: 6),
                decoration: BoxDecoration(
                  color: light.isOn ? Colors.green : Colors.red,
                  borderRadius: BorderRadius.circular(20),
                ),
                child: Text(light.isOn ? 'ON' : 'OFF', style: TextStyle(color: Colors.white, fontWeight: FontWeight.bold)),
              ),
            ],
          ),
        ),
      ),
    );
  }

  @override
  void dispose() {
    client?.disconnect();
    super.dispose();
  }
}
