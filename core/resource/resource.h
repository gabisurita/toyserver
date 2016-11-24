
/* Function test_resource()
* Verify if a resource on a given path exists and if it is accessable
*
* Parameters:
*   ServerRoot: webspace root address
*
*/

int test_resource(char *__server_root, char *resource);

char* get_resource_path();

struct stat get_resource_status();
