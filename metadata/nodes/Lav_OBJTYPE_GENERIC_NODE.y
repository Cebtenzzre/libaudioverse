suppress_implied_inherit: true
properties:
  Lav_NODE_STATE:
    name: state
    type: int
    default: Lav_NODESTATE_PLAYING
    value_enum: Lav_NODE_STATES
    doc_description: |
      The node's state.  See the basics section in the Libaudioverse manual for details.
      The default is usually what you want.
  Lav_NODE_AUTORESET:
    name: autoreset
    type: boolean
    default: 1
    doc_description: |
      Every node has a reset function which does something specific to the node.
      If this property is enabled, then changes to the inputs of this node will cause it to automatically reset.
  Lav_NODE_MUL:
    name: mul
    type: float
    default: 1.0
    range: [-INFINITY, INFINITY]
    doc_description: |
      After this node processes, the value to which mul is set is used as a multiplier on the result.
      The most notable effect of this is to change the node's volume.
      A variety of other uses exist, however, especially as regards to nodes which are connected to properties.
      Mul is applied before add.
  Lav_NODE_ADD:
    name: add
    type: float
    default: 0.0
    range: [-INFINITY, INFINITY]
    doc_description: |
      After mul is applied, we add the value to which this property is set to the node's result.
      The purpose of add is to be used with automation support: add provides a baseline for oscillators and other similar nodes
      so that when connected to properties, we can make the value of this node vary between a specified minimum and maximum.
  Lav_NODE_CHANNEL_INTERPRETATION:
    name: channel_interpretation
    type: int
    value_enum: Lav_CHANNEL_INTERPRETATIONS
    doc_description: |
      This property is not currently used, and is reserved for future use.
doc_name: generic
doc_description: |
  The properties and functionality described here are available on every Libaudioverse node without exception.