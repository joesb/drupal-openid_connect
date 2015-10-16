<?php

/**
 * @file
 * Contains \Drupal\openid_connect\Plugin\OpenIDConnectClient\OpenIDConnectClientGoogle.
 */

namespace Drupal\openid_connect\Plugin\OpenIDConnectClient;

use Drupal\openid_connect\Plugin\OpenIDConnectClientBase;

/**
 * OpenID Connect client for Google.
 *
 * Implements OpenID Connect Client plugin for Google.
 *
 * @OpenIDConnectClient(
 *   id = "google",
 *   label = @Translation("Google OpenID Connect client")
 * )
 */
class OpenIDConnectClientGoogle extends OpenIDConnectClientBase {

  /**
   * Overrides OpenIDConnectClientBase::getEndpoints().
   */
  public function getEndpoints() {
    return array(
      'authorization' => $this->getSetting('authorization_endpoint'),
      'token' => $this->getSetting('token_endpoint'),
      'userinfo' => $this->getSetting('userinfo_endpoint'),
    );
  }

  /**
   * Overrides OpenIDConnectClientBase::retrieveUserInfo().
   */
  public function retrieveUserInfo($access_token) {
    $userinfo = parent::retrieveUserInfo($access_token);
    if ($userinfo) {
      // For some reason Google returns the URI of the profile picture in a
      // weird format: "https:" appears twice in the beginning of the URI.
      // Using a regular expression matching for fixing it guarantees that
      // things won't break if Google changes the way the URI is returned.
      preg_match('/https:\/\/*.*/', $userinfo['picture'], $matches);
      $userinfo['picture'] = $matches[0];
    }

    return $userinfo;
  }
}
