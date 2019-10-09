<?php

/**
 * @file
 * Block Proxies Administration menu functions.
 *
 * @license http://www.gnu.org/copyleft/gpl.html GNU General Public License
 */

/**
 * Implements hook_page().
 */
function block_proxies_page() {
  $build['block_proxies_form'] = drupal_get_form('block_proxies_form');

  return $build;
}

/**
 * Implements hook_form().
 */
function block_proxies_form($form, $form_state) {
  $result = db_select('block_proxies', 'bp')->fields('bp')->execute()->fetchAssoc();

  // Initialize default values.
  $net_dnsbl_noexists = FALSE;
  $field_values_anonymouse = '';
  $field_values_anonymouse_reason = '';
  $field_values_google_proxy = '';
  $field_values_google_proxy_reason = '';
  $field_values_dnsbl = '';
  $field_values_dnsbl_list = '';
  $field_values_dnsbl_reason = '';

  @include_once 'Net/DNSBL.php';

  if (!class_exists('Net_DNSBL')) {
    drupal_set_message(l(t('Net_DNSBL'), 'https://pear.php.net/package/Net_DNSBL/', array('external' => TRUE)) . t('Library not installed'), 'error');
    $net_dnsbl_noexists = TRUE;
  }

  if (isset($result)) {
    $field_values = unserialize($result['configuration']);
  }

  if (isset($field_values)) {
    if (isset($field_values['anonymouse'])) {
      $field_values_anonymouse = $field_values['anonymouse'];
    }
    else {
      $field_values_anonymouse = FALSE;
    }

    if (isset($field_values['anonymouse_reason'])) {
      $field_values_anonymouse_reason = $field_values['anonymouse_reason'];
    }
    else {
      $field_values_anonymouse_reason = 'Anonymouse use is blocked';
    }

    if (isset($field_values['google_proxy'])) {
      $field_values_google_proxy = $field_values['google_proxy'];
    }
    else {
      $field_values_google_proxy = FALSE;
    }

    if (isset($field_values['google_proxy_reason'])) {
      $field_values_google_proxy_reason = $field_values['google_proxy_reason'];
    }
    else {
      $field_values_google_proxy_reason = 'Google as Proxy is blocked';
    }

    if (isset($field_values['dnsbl'])) {
      $field_values_dnsbl = $field_values['dnsbl'];
    }
    else {
      $field_values_dnsbl = FALSE;
    }

    if (isset($field_values['dnsbl_list'])) {
      $field_values_dnsbl_list = $field_values['dnsbl_list'];
    }
    else {
      $field_values_dnsbl_list = FALSE;
    }

    if (isset($field_values['dnsbl_reason'])) {
      $field_values_dnsbl_reason = $field_values['dnsbl_reason'];
    }
    else {
      $field_values_dnsbl_reason = 'Your DNSBL is Blacklisted';
    }
  }

  $form['anonymouse'] = array(
    '#type' => 'checkbox',
    '#title' => t('Anonymouse'),
    '#required' => FALSE,
    '#default_value' => $field_values_anonymouse,
  );

  $form['anonymouse_reason'] = array(
    '#type' => 'textfield',
    '#required' => FALSE,
    '#default_value' => $field_values_anonymouse_reason,
    '#states' => array(
      'visible' => array(
        ':input[id=edit-anonymouse]' => array('checked' => TRUE),
      ),
    ),
  );

  $form['google_proxy'] = array(
    '#type' => 'checkbox',
    '#title' => t('Google as Proxy'),
    '#required' => FALSE,
    '#default_value' => $field_values_google_proxy,
  );

  $form['google_proxy_reason'] = array(
    '#type' => 'textfield',
    '#title' => t('Google as Proxy Reason'),
    '#required' => FALSE,
    '#default_value' => $field_values_google_proxy_reason,
    '#states' => array(
      'visible' => array(
        ':input[id=edit-google-proxy]' => array('checked' => TRUE),
      ),
    ),
  );

  $form['dnsbl'] = array(
    '#type' => 'checkbox',
    '#title' => t('DNSBL'),
    '#required' => FALSE,
    '#default_value' => $field_values_dnsbl,
  );

  $form['dnsbl_list'] = array(
    '#type' => 'textfield',
    '#title' => t('DNSBL Check Servers - Commma delimited'),
    '#default_value' => $field_values_dnsbl_list,
    '#disabled' => $net_dnsbl_noexists,
    '#states' => array(
      'visible' => array(
        ':input[id=edit-dnsbl]' => array('checked' => TRUE),
      ),
    ),
  );

  $form['dnsbl_reason'] = array(
    '#type' => 'textfield',
    '#title' => t('DNSBL Servers Reason'),
    '#default_value' => $field_values_dnsbl_reason,
    '#states' => array(
      'visible' => array(
        ':input[id=edit-dnsbl]' => array('checked' => TRUE),
      ),
    ),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * Implements hook_from_submit().
 */
function block_proxies_form_submit($form, &$form_state) {
  $form_state['values']['dnsbl'] = str_replace(' ', '', $form_state['values']['dnsbl']);

  $data = serialize($form_state['values']);

  $result = db_select('block_proxies', 'bp')->fields('bp')->execute();
  $num_of_results = $result->rowCount();

  if ($num_of_results == 0) {
    db_insert('block_proxies')->fields(array('site' => 'all', 'configuration' => $data))->execute();
  }
  else {
    db_update('block_proxies')->fields(array('configuration' => $data))->condition('site', 'all')->execute();
  }

  drupal_set_message(t('Configuration Updated'));
  $form_state['redirect'] = 'admin/config/people/block-proxies';

  // return;.
}
