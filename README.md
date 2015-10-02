# OpenID Connect

The OpenID Connect module provides a pluggable client implementation for the
OpenID Connect protocol.

For more information please consult the documentation: https://drupal.org/node/2274339

## Drupal 5 Backport

The Drupal 5.x backport differs in a few ways from the Drupal 7 implementation.

- CTools doesn't exist in Drupal 5, so instead Drupal's hook system provides opportunities for other modules to register
- `hook_username_alter()` doesn't exist in D5, so that functionality is absent from the D5 module
- `tfa` module hasn't been backported to D5. The code referencing it is present in the D5 module, but no guarantees that it would work with a ported TFA module
- The module supports mapping claims for fields added by the core Profile module

### Testing

Creating a PHP 5.2 environment is a little tricky in the year 2015. I recommend using the [WordPress Vagrant Boxes](https://github.com/tierra/wp-vagrant) project. If you go that route, and you're planning to test your implementation against Google's OpenID Connect implementation, you'll want to do these steps:

1. Edit the Vagrantfile and adjust `node.vm.hostname` so the line reads `node.vm.hostname = 'wordpress-php52.com`
2. `vagrant up wordpress-php52`
3. `vagrant ssh wordpress-php52`
4. `sudo nano /etc/apache2/sites-available/25-wordpress.conf`
5. Add a line for `ServerAlias wordpress-php52.com`
6. Restart apache2
7. On the host machine, add an entry in `/etc/hosts` for `192.168.167.9 wordpress-php52.local wordpress-php52.com`

The reason you need to do this is because Google doesn't support `.local` as a valid domain in their developer console.
