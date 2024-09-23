# include <stdio.h>
# include <unistd.h>
# include <nrm.h>

int main()
{
  int i, num_logical_cpus, measurement;

  nrm_client_t *client;
  nrm_scope_t *scope;
  nrm_sensor_t *sensor;
  nrm_time_t timestamp;

  static char *upstream_uri = "tcp://127.0.0.1";
  static int pub_port = 2345;
  static int rpc_port = 3456;
  char *sensor_name = "example-measure";
  char *scope_name = "my.scope.name";

  nrm_init(NULL, NULL);

  nrm_client_create(&client, upstream_uri, pub_port, rpc_port);

  scope = nrm_scope_create(scope_name);
        sensor = nrm_sensor_create(sensor_name);

  nrm_client_add_scope(client, scope);
  nrm_client_add_sensor(client, sensor);

  num_logical_cpus = example_get_num_logical_cpus();
  for (int i=0; i<num_logical_cpus; i++) {
    nrm_scope_add(scope, NRM_SCOPE_TYPE_CPU, i);
  }

  printf("hello\n")

  do {
    measurement = example_get_cpu_measurement();
    nrm_time_gettime(&timestamp);
    nrm_client_send_event(client, timestamp, sensor, scope, measurement);
    sleep(1);
  } while (measurement != 0);

  printf("done!");

  nrm_scope_destroy(scope);
        nrm_sensor_destroy(&sensor);
        nrm_client_destroy(&client);
        nrm_finalize();

  exit(EXIT_SUCCESS);
}
