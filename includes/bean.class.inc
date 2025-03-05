<?php

/**
 * @file
 * Bean classes and plugin interface
 */

/**
 * The Bean entity class
 */
class Bean extends Entity {
  protected $entityInfo; // Stores the info provided in hook_entity_info() for this entity.
  protected $idKey; // Stores the key where the id is stored.
  protected $nameKey; // Stores the key where the name of the entity is stored.
  protected $plugin;

  public $label;
  public $description;
  public $title;
  public $data;
  public $delta;
  public $view_mode;
  public $bid;
  public $vid;
  public $log;
  public $uid;
  public $default_revision;
  public $revisions = array();

  /**
   * The Unix timestamp for the date the Bean was created.
   *
   * @var integer
   */
  public $created;

  /**
   * The Unix timestamp for the date the Bean was changed/modified.
   *
   * @var integer
   */
  public $changed;

  /**
   * The Bean type.
   *
   * @var string
   */
  public $type;

  /**
 *
 */
  public function __construct(array $values = array()) {
    // If this is new then set the type first.
    if (isset($values['is_new'])) {
      $this->type = isset($values['type']) ? $values['type'] : '';
    }

    parent::__construct($values, 'bean');
    $this->entityType = $this->entityType();
    $this->entityInfo = entity_get_info($this->entityType());
    $this->idKey = $this->entityInfo['entity keys']['id'];
    $this->nameKey = $this->entityInfo['entity keys']['name'];
  }

  /**
   * {@inheritdoc}
   *
   * Returns the id of this entity.
   */
  public function internalIdentifier() {
    return isset($this->{$this->idKey}) ? $this->{$this->idKey} : NULL;
  }

  /**
   * Returns the delta of this entity.
   */
  public function identifier() {
    return isset($this->{$this->nameKey}) ? $this->{$this->nameKey} : NULL;
  }

  /**
   * Implements EntityInterface::id().
   *
   * Returns the entity identifier (the entity's machine name or numeric ID).
   *
   * @return
   *   The identifier of the entity, or NULL if the entity does not yet have
   *   an identifier.
   */
  public function id() {
    return $this->internalIdentifier();
  }

  /**
   * Implements EntityInterface::uri().
   */
  function uri() {
    return array(
      'path' => 'block/' . strtolower($this->identifier()),
      'options' => array(),
    );
  }

  /**
   * Implements EntityInterface::entityType().
   */
  public function entityType() {
    return 'bean';
  }

  /**
   * Implements EntityInterface::label().
   *
   * Return the human label for this entity.
   */
  function label() {
    return $this->{$this->entityInfo['entity keys']['label']};
  }

  /**
   * Implements EntityInterface::bundle().
   */
  public function bundle() {
    return $this->type;
  }

  /**
 * Return a sanitized version of the Bean admin label, or an empty string.
 */
  public function getLabel() {
    return (!empty($this->label)) ? filter_xss_admin($this->label) : '';
  }

  /**
 * Return a sanitized version of the Bean title, with allowed HTML elements, or an empty string.
 */
  public function getTitle() {
    return (!empty($this->title)) ? filter_xss($this->title, $allowed_tags = array('em', 'strong', 'i', 'b')) : '';
  }

  /**
 *
 */
  protected function setUp() {
    parent::setUp();

    if (!empty($this->type)) {
      $plugin = bean_load_plugin_class($this->type);
      $this->loadUp($plugin);
    }
    return $this;
  }

  /**
   * This is a work around for version of PDO that call __construct() before
   * it loads up the object with values from the DB.
   */
  public function loadUp($plugin) {
    if (empty($this->plugin)) {
      // If the plugin has been disabled, the $plugin will be a boolean.
      // just load the base plugin so something will render.
      if (!($plugin instanceof BeanTypePluginInterface)) {
        $plugin = bean_load_plugin_class('bean_default');
      }

      $this->setPlugin($plugin)->setFields();
    }
    return $this;
  }

  /**
   * Load and set the plugin info.
   * This can be called externally via loadUP()
   */
  protected function setPlugin($plugin) {
    $this->plugin = $plugin;

    return $this;
  }

  /**
   * Set the fields from the defaults and plugin.
   * This can be called externally via loadUP() (NOTE: When setFields is called
   * externally $this->data is already unserialized).
   */
  protected function setFields() {
    if (empty($this->data)) {
      $this->data = array();
    }
    if (!empty($this->plugin) && !empty($this->type)) {
      $values = is_array($this->data) ? $this->data : unserialize($this->data);
      foreach ($this->plugin->values() as $field => $default) {
        $this->$field = isset($values[$field]) ? $values[$field] : $default;
      }
    }
    return $this;
  }

  /**
   * Get the default label
   *
   * @return string
   */
  protected function defaultLabel() {
    if (empty($this->title)) {
      return $this->getLabel();
    }

    return $this->getTitle();
  }

  /**
   * Set the revision as default
   *
   * @return Bean
   */
  public function setDefault() {
    $this->default_revision = TRUE;
    return $this;
  }

  /**
   * Get Plugin info
   *
   * @param $key
   *   The plugin key to get the info for.
   *
   * @return array|null
   */
  public function getInfo($key = NULL) {
    if ($this->plugin instanceof BeanTypePluginInterface) {
      return $this->plugin->getInfo($key);
    }

    return NULL;
  }

  /**
   * Get the plugin form
   */
  public function getForm($form, &$form_state) {
    return $this->plugin->form($this, $form, $form_state);
  }

  /**
   * Validate the plugin form
   */
  public function validate($values, &$form_state) {
    $this->plugin->validate($values, $form_state);
  }

  /**
   * URL string
   */
  public function url() {
    $uri = $this->uri();
    return $uri['path'];
  }

  /**
   * Build the URL string
   */
  protected function buildURL(string $type) {
    $url = $this->url();
    return $url . '/' . $type;
  }

  /**
   * View URL
   */
  public function viewURL() {
    return $this->buildURL('view');
  }

  /**
   * Edit URL
   */
  public function editURL() {
    return $this->buildURL('edit');
  }

  /**
   * Delete URL
   */
  public function deleteURL() {
    return $this->buildURL('delete');
  }

  /**
   * Type name
   */
  public function typeName() {
    return $this->getInfo('label');
  }

  /**
   * Set the values from a plugin
   */
  public function setValues($values) {
    $this->data = array();
    foreach ($this->plugin->values() as $field => $value) {
      $this->data[$field] = $values['bean']->{$field};
    }

    return $this;
  }

  /**
   * Generate an array for rendering the entity.
   *
   * Overrides EntityInterface::view().
   */
  public function view($view_mode = 'default', $langcode = NULL, $page = NULL) {
    $view = entity_get_controller($this->entityType())->view(array($this->id() => $this), $view_mode, $langcode, $page);
    $content = $view[$this->entityType()][$this->id()];
    if (empty($this->plugin)) {
      return $content;
    }
    $content = $this->plugin->view($this, $content, $view_mode, $langcode);
    return $this->plugin->view($this, $content, $view_mode, $langcode);
  }

  /**
   * Override the save to add clearing of caches
   */
  public function save() {
    $this->setUid()->checkDelta();

    if (empty($this->created)) {
      $this->created = REQUEST_TIME;
    }

    $this->changed = REQUEST_TIME;

    $this->plugin->submit($this);

    $return = parent::save();
    bean_reset(TRUE, TRUE);

    if (module_exists('block')) {
      bean_cache_clear_block($this);
    }

    return $return;
  }

  /**
   * Get the user
   */
  protected function setUid() {
    global $user;

    $this->uid = empty($this->uid) ? $user->uid : $this->uid;

    return $this;
  }

  /**
   * Set the delta function
   */
  protected function checkDelta() {
    if (empty($this->delta)) {
      $max_length = 32;

      // Base it on the label and make sure it isn't too long for the database.
      if (module_exists('transliteration')) {
        $delta = backdrop_clean_css_identifier(strtolower(transliteration_get($this->label)));
      }
      else {
        $delta = backdrop_clean_css_identifier(strtolower($this->label));
      }

      // Add 'bean-' prefix when it's need to avoid a purely numeric delta.
      if (is_numeric($delta)) {
        $delta = 'bean-' . $delta;
      }

      $this->delta = substr($delta, 0, $max_length);

      // Check if delta is unique.
      if ($this->deltaExists($this->delta)) {
        $i = 0;
        $separator = '-';
        $original_delta = $this->delta;
        do {
          $unique_suffix = $separator . $i++;
          $this->delta = substr($original_delta, 0, $max_length - backdrop_strlen($unique_suffix)) . $unique_suffix;
        } while ($this->deltaExists($this->delta));
      }
    }

    return $this;
  }

  /**
   * Check against the Database if a specific delta exists.
   */
  protected function deltaExists($delta) {
    return db_select('bean', 'b')
      ->fields('b', array('bid'))
      ->condition('delta', $delta)
      ->execute()->fetchField();
  }

  /**
   * Load the revisions from the DB
   */
  public function loadRevisions() {
    $this->revisions = db_select('bean_revision', 'br')
                        ->condition('delta', $this->identifier())
                        ->fields('br')
                        ->execute();

    return $this;
  }
}
