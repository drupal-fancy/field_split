<?php

/**
 * @file
 * Administrative interface for Field Split.
 */

/**
 * Configuration form.
 */
function field_split_config_form($form, $form_state) {
  $form['#tree'] = TRUE;
  $form['#attached']['css'][] = drupal_get_path('module', 'field_split') . '/css/field-split.css';
  $field_info = field_info_fields();
  $settings = variable_get('field_split_config', array());

  foreach ($field_info as $field) {
    foreach ($field['bundles'] as $entity_type => $bundles) {
      foreach ($bundles as $bundle) {
        // Field split can be only available for fields w/ more than one value.
        if ($field['cardinality'] == 1) {
          continue;
        }

        $field_info = field_info_instance($entity_type, $field['field_name'], $bundle);
        $label = $field_info['label'];
        $total_fields = $field['cardinality'] != FIELD_CARDINALITY_UNLIMITED ? $field['cardinality'] : 50;

        $options = array();

        // Build a list with available values.
        foreach(range(2, $total_fields) as $number) {
          $options[$number] = $number;
        }

        if (!isset($settings[$entity_type][$bundle][$field['field_name']])) {
          $settings[$entity_type][$bundle][$field['field_name']] = array(
            'enable' => FALSE,
            'values' => 1,
          );
        }

        if (!isset($form['entity_types'][$entity_type])) {
          $form['entity_types'][$entity_type] = array(
            '#type' => 'fieldset',
            '#title' => $entity_type,
          );
        }

        if (!isset($form['entity_types'][$entity_type][$bundle])) {
          $form['entity_types'][$entity_type][$bundle] = array(
            '#type' => 'fieldset',
            '#title' => $bundle,
            '#collapsible' => TRUE,
            '#collapsed' => TRUE,
          );
        }

        $form['entity_types'][$entity_type][$bundle][$field['field_name']]['enable'] = array(
          '#title' => t('Split %field (@machine-name)', array('%field' => $label, '@machine-name' => $field['field_name'])),
          '#type' => 'checkbox',
          '#default_value' => $settings[$entity_type][$bundle][$field['field_name']]['enable'],
        );

        $form['entity_types'][$entity_type][$bundle][$field['field_name']]['values'] = array(
          '#title' => t('Values'),
          '#type' => 'select',
          '#options' => $options,
          '#default_value' => $settings[$entity_type][$bundle][$field['field_name']]['values'],
          '#description' => t('Select the amount of fields in total.'),
          '#states' => array(
            'visible' => array(
              'input[name$="entity_types[' . $entity_type . '][' . $bundle . '][' . $field['field_name'] . '][enable]"]' => array('checked' => TRUE),
            ),
          ),
        );
      }
    }
  }

  $form['actions'] = array(
    '#type' => 'actions',
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );

  return $form;
}

/**
 * Submit function for configuration form.
 */
function field_split_config_form_submit($form, &$form_state) {
  variable_set('field_split_config', $form_state['values']['entity_types']);

  drupal_set_message(t('The configuration options have been saved.'));
}
