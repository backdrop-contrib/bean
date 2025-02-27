<?php

/**
 * @file
 * Bean plugin
 */

/**
 * DO NOT USE THIS BEAN.  ONLY USED FOR THE UI PLUGINS
 */
class BeanCustom extends BeanPlugin {
  /**
   * Delete the Bean type from config.
   */
  public function delete() {
    $config = config('bean.type.' . $this->type);
    $config->delete();

    field_attach_delete_bundle('bean', $this->type);

    bean_reset();
  }

  /**
   * Save the Bean type to config.
   */
  public function save($new = FALSE) {
    $config = config('bean.type.' . $this->type);
    $config->set('name', check_plain($this->name));
    $config->set('label', check_plain($this->getLabel()));
    $config->set('description', check_plain($this->getDescription()));
    $config->set('storage_status', BEAN_STORAGE_OVERRIDE);
    $config->save();
    bean_reset();
  }

  /**
   * Revert the Bean type to code defaults.
   */
  public function revert() {
    // ctools_include('export');
    // ctools_export_crud_delete('bean_type', $this->type);.
    bean_reset();
  }

  /**
   * Get the export status code.
   */
  public function getStorageStatus() {
    return $this->plugin_info['storage_status'];
  }

  /**
   * Get the export status front-facing label.
   */
  public function getStorageStatusLabel() {
    $storage_code = $this->getStorageStatus();
    $storage_labels = array(
      BEAN_STORAGE_NORMAL => t('User-defined'),
      BEAN_STORAGE_OVERRIDE => t('Module-defined (override)'),
      BEAN_STORAGE_DEFAULT => t('Module-defined'),
    );

    return isset($storage_labels[$storage_code]) ? $storage_labels[$storage_code] : t('Unknown');
  }

  /**
   * Set the label.
   *
   * @param label
   */
  public function setLabel($label) {
    $this->plugin_info['label'] = $label;
    config_set('bean.type.' . $this->type, 'label', $label);
  }

  /**
   * Set the description.
   *
   * @param description
   */
  public function setDescription($description) {
    $this->plugin_info['description'] = $description;
    config_set('bean.type.' . $this->type, 'description', $description);
  }

  /**
   * Build the URL string
   */
  public function buildURL() {
    return str_replace('_', '-', $this->type);
  }

  /**
   * View
   */
  public function view($bean, $content, $view_mode = 'default', $langcode = NULL) {
    return $content;
  }

}
