<?php

/**
 * @file
 * Forms for OpenID Connect.
 */

/**
 * Form builder: Log in with an external account.
 */
function openid_connect_login_form() {
  $form = array();
  include_once './' . drupal_get_path('module', 'openid_connect') . '/includes/openid_connect.forms.inc';

  foreach (openid_connect_get_plugins(TRUE) as $client) {
    $form['openid_connect_client_' . $client['name'] . '_login'] = array(
      '#type' => 'submit',
      '#value' => t('Log in with !client_title', array('!client_title' => $client['title'])),
      '#name' => $client['name'],
    );
  }

  return $form;
}

/**
 * Form submit handler: Log in with an OpenID Connect login provider.
 */
function openid_connect_login_form_submit($form_id, $form_values) {
  openid_connect_save_destination();
  // N.B. This is ugly, but not sure if there's a better way.
  $client_name = '';
  foreach ($form_values as $key => $value) {
    if (strpos($key, 'openid_connect_client_') === 0) {
      continue;
    }
    if ($key == 'form_id') {
      continue;
    }
    $client_name = $key;
  }
  $client = openid_connect_get_client($client_name);
  $scopes = openid_connect_get_scopes();
  $_SESSION['openid_connect_op'] = 'login';
  $client->authorize($scopes);
}

/**
 * Form builder: Connect an external account to your existing Drupal account.
 */
function openid_connect_connect_form() {
  include_once realpath('.') . '/' . drupal_get_path('module', 'openid_connect') . '/includes/openid_connect.forms.inc';

  $form['#tree'] = TRUE;

  $account = user_load(array('uid' => arg(1)));
  $GLOBALS['form_state']['account'] = $account;

  $clients = openid_connect_get_plugins(TRUE);

  $read_only = $GLOBALS['user']->uid != $account->uid;

  $form['help'] = array(
    '#prefix' => '<p class="description">',
    '#suffix' => '</p>',
  );

  if (empty($clients)) {
    $form['help']['#value'] = t('No external account providers are available.');
    return $form;
  }
  elseif ($GLOBALS['user']->uid == $account->uid) {
    $form['help']['#value'] = t('You can connect your account with these external providers.');
  }

  $connected_accounts = openid_connect_get_connected_accounts($account);

  foreach ($clients as $client) {
    $form[$client['name']] = array(
      '#type' => 'fieldset',
      '#title' => t('Provider: @title', array('@title' => $client['title'])),
    );
    $fieldset = &$form[$client['name']];
    $connected = isset($connected_accounts[$client['name']]);
    $fieldset['status'] = array(
      '#type' => 'item',
      '#title' => t('Status'),
      '#value' => t('Not connected'),
    );
    if ($connected) {
      $fieldset['status']['#value'] = t('Connected as %sub',
                                      array(
                                        '%sub' => $connected_accounts[$client['name']],
                                      ));
      $fieldset['openid_connect_client_' . $client['name'] . '_disconnect'] = array(
        '#type' => 'submit',
        '#value' => t('Disconnect from !client_title', array('!client_title' => $client['title'])),
        '#name' => 'disconnect__' . $client['name'],
        '#access' => !$read_only,
      );
    }
    else {
      $fieldset['openid_connect_client_' . $client['name'] . '_connect'] = array(
        '#type' => 'submit',
        '#value' => t('Connect with !client_title', array('!client_title' => $client['title'])),
        '#name' => 'connect__' . $client['name'],
        '#access' => !$read_only,
      );
    }
  }

  return $form;
}

/**
 * Submit callback for the connect account form.
 *
 * Connect the current user's Drupal account to an external provider.
 */
function openid_connect_connect_form_submit(&$form, $form_values) {

  list($op, $client_name) = explode('__', array_shift(array_keys($form_values)), 2);
  $form_state = $GLOBALS['form_state'];
  if ($op === 'disconnect') {
    openid_connect_disconnect_account($form_state['account'], $client_name);
    $client = openid_connect_get_client($client_name);
    drupal_set_message(t('Account successfully disconnected from @client.', array('@client' => $client->getLabel())));

    return;
  }

  global $user;

  if ($user->uid !== $form_state['account']->uid) {
    drupal_set_message(t("You cannot connect another user's account."), 'error');

    return;
  }

  openid_connect_save_destination();

  $client = openid_connect_get_client($client_name);
  $scopes = openid_connect_get_scopes();
  $_SESSION['openid_connect_op'] = $op;
  $_SESSION['openid_connect_connect_uid'] = $user->uid;
  $client->authorize($scopes);
}
